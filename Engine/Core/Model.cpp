#include "Model.h"
#include "Engine/Loaders/Loader.hpp"
#include "Engine/Loaders/vboindexer.h"
#include "Engine/Loaders/stb_image.h"
#include "Engine/Core/AssetManager.h"

Model::Model(Mesh mesh, Texture* texture) {
    mesh.SetTexture(texture);
    meshes.push_back(mesh);
}

//Right now this can only load files that support tanget and bit tanget fbx is the most common
Model::Model(const char* path, Texture* texture) {
    Assimp::Importer import;
    const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    std::cout << "Assimp: Loading Model " << path << std::endl;
    processNode(scene->mRootNode, scene, texture);

}
Model::Model(const char* path, const char* collisonShapePath, Texture* texture) {
    Assimp::Importer import;
    const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    std::cout << "Assimp: Loading Model " << path << std::endl;
    processNode(scene->mRootNode, scene, texture);

    //Ill make a new loadObj that dosent next uvs and normals but this is just for testing right now
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    Loader::loadOBJ(collisonShapePath, collison_shape_vertices, uvs, normals);
}


Mesh Model::processMesh(aiMesh* mesh, const aiScene* scenem) {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> UV;
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitTangents;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertices.push_back(vector);
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            normals.push_back(vector);
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            UV.push_back(vec);

            if (mesh->HasTangentsAndBitangents())
            {
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                tangents.push_back(vector);
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                bitTangents.push_back(vector);
            }
        }
        else
            UV.push_back(glm::vec2(0.0f, 0.0f));
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    //Computer tanget and bit-tangent if model doesnt have them
    //TODO: Fix this because it keeps trying to access memory past vertcies size
    if (!mesh->HasTangentsAndBitangents())
    {
        for (int i = 0; i < vertices.size() - 2; i += 3) {
            // Shortcuts for vertices
            glm::vec3& v0 = vertices[i + 0];
            glm::vec3& v1 = vertices[i + 1];
            glm::vec3& v2 = vertices[i + 2];

            // Shortcuts for UVs
            glm::vec2& uv0 = UV[i + 0];
            glm::vec2& uv1 = UV[i + 1];
            glm::vec2& uv2 = UV[i + 2];
            // Edges of the triangle : position delta
            glm::vec3 deltaPos1 = v1 - v0;
            glm::vec3 deltaPos2 = v2 - v0;

            // UV delta
            glm::vec2 deltaUV1 = uv1 - uv0;
            glm::vec2 deltaUV2 = uv2 - uv0;

            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
            glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

            // Set the same tangent for all three vertices of the triangle.
            // They will be merged later, in vboindexer.cpp
            tangents.push_back(tangent);
            tangents.push_back(tangent);
            tangents.push_back(tangent);

            // Same thing for bitangents
            bitTangents.push_back(bitangent);
            bitTangents.push_back(bitangent);
            bitTangents.push_back(bitangent);
        }
    }
    return Mesh(vertices, normals, UV, indices, tangents, bitTangents);
}
void Model::processNode(aiNode* node, const aiScene* scene, Texture* texture) {
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh model_mesh = processMesh(mesh, scene);
        model_mesh.SetTexture(texture);
        model_mesh.SetName(mesh->mName.C_Str());
        meshes.push_back(model_mesh);
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, texture);
    }
}
void Model::AddMesh(Mesh mesh) {
    meshes.push_back(mesh);
}
void Model::SetMesh(int mesh) {
    if (mesh >= 0 && mesh < meshes.size())
        currentMesh = mesh;
}
Mesh* Model::GetCurrentMesh() {
    return &meshes[currentMesh];
}
Mesh* Model::GetMeshByName(std::string name) {
    for (int i = 0; i < meshes.size(); i++) {
        if (meshes[i].GetName() == name)
            return &meshes[i];
    }
    return nullptr;
}

size_t Model::GetMeshSize() {
    return meshes.size();
}
Mesh* Model::GetMesh(int i) {
    return &meshes[i];
}

std::vector<Mesh>* Model::GetAllMeshes() {
    return &meshes;
}

void Model::RenderAllMeshes(bool state) {
    renderAllMeshes = state;
}
bool Model::RenderAll() {
    return renderAllMeshes;
}

void Model::RenderModel(GLuint& programID) {

    if (renderAllMeshes)
    {
        for (int i = 0; i < meshes.size(); i++)
            meshes[i].Render(programID);
    }
    else
        meshes[currentMesh].Render(programID);
}
size_t Model::GetColliderShapeVerticiesSize() {
    return collison_shape_vertices.size();
}

std::vector<glm::vec3> Model::GetColliderShapeVerticies() {
    return collison_shape_vertices;
}