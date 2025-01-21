#include "Mesh.h"
#include "Engine/Loaders/Loader.hpp"
#include "Engine/Loaders/vboindexer.h"
#include "Engine/Loaders/stb_image.h"
#include "Engine/Core/AssetManager.h"




Mesh::Mesh(const char* path) {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;

    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;

    bool res = Loader::loadOBJ(path, vertices, uvs, normals);

    //compute tangents
    for (int i = 0; i < vertices.size(); i += 3) {
        // Shortcuts for vertices
        glm::vec3& v0 = vertices[i + 0];
        glm::vec3& v1 = vertices[i + 1];
        glm::vec3& v2 = vertices[i + 2];

        // Shortcuts for UVs
        glm::vec2& uv0 = uvs[i + 0];
        glm::vec2& uv1 = uvs[i + 1];
        glm::vec2& uv2 = uvs[i + 2];
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
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
    }

    indexer::indexVBO(vertices, uvs, normals, tangents, bitangents, indices, indexed_vertices, indexed_uvs, indexed_normals, indexed_tangents, indexed_bitangents);

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &tangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_tangents.size() * sizeof(glm::vec3), &indexed_tangents[0], GL_STATIC_DRAW);

    glGenBuffers(1, &bitangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_bitangents.size() * sizeof(glm::vec3), &indexed_bitangents[0], GL_STATIC_DRAW);

    for (unsigned int i = 0; i < indexed_vertices.size(); i++)
    {
        indexed_jointIDs.push_back(glm::ivec4(-1));
        indexed_weights.push_back(glm::vec4(0.0f));
    }
    glGenBuffers(1, &jointIdbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, jointIdbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_jointIDs.size() * sizeof(glm::ivec4), &indexed_jointIDs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &Weightbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, Weightbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_weights.size() * sizeof(glm::vec4), &indexed_weights[0], GL_STATIC_DRAW);


}

Mesh::Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> UV, std::vector<unsigned short> indices, std::vector<glm::vec3> tangets, std::vector<glm::vec3> bitTangents) {
    this->indexed_vertices = vertices;
    this->indexed_normals = normals;
    this->indexed_uvs = UV;
    this->indexed_bitangents = bitTangents;
    this->indexed_tangents = tangets;
    this->indices = indices;

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &tangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_tangents.size() * sizeof(glm::vec3), &indexed_tangents[0], GL_STATIC_DRAW);

    glGenBuffers(1, &bitangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_bitangents.size() * sizeof(glm::vec3), &indexed_bitangents[0], GL_STATIC_DRAW);
}

Mesh::Mesh(std::vector<glm::vec3> vertices,
    std::vector<glm::vec3> normals,
    std::vector<glm::vec2> UV,
    std::vector<unsigned short> indices,
    std::vector<glm::vec3> tangets,
    std::vector<glm::vec3> bitTangents,
    std::vector<glm::ivec4> jointIDs,
    std::vector<glm::vec4> weights) {

    this->indexed_vertices = vertices;
    this->indexed_normals = normals;
    this->indexed_uvs = UV;
    this->indexed_bitangents = bitTangents;
    this->indexed_tangents = tangets;
    this->indices = indices;
    this->indexed_jointIDs = jointIDs;
    this->indexed_weights = weights;

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &tangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_tangents.size() * sizeof(glm::vec3), &indexed_tangents[0], GL_STATIC_DRAW);

    glGenBuffers(1, &bitangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_bitangents.size() * sizeof(glm::vec3), &indexed_bitangents[0], GL_STATIC_DRAW);

    glGenBuffers(1, &jointIdbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, jointIdbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_jointIDs.size() * sizeof(glm::ivec4), &indexed_jointIDs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &Weightbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, Weightbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_weights.size() * sizeof(glm::vec4), &indexed_weights[0], GL_STATIC_DRAW);


}


void Mesh::Render(GLuint programID) {
    Texture* currentTexture = texture;
    if (texture == nullptr) {
        currentTexture = AssetManager::GetMissingTexture();
    }


    if (Renderer::GetCurrentProgramID() != Renderer::s_SolidColor.GetShaderID()) {
        glBindTextureUnit(0, currentTexture->GetTexture());

        if (currentTexture->GetTextureNormal() == NULL)
            glUniform1i(glGetUniformLocation(programID, "HasNormalMap"), false);
        else
            glBindTextureUnit(1, currentTexture->GetTextureNormal());

        glBindTextureUnit(2, currentTexture->GetTextureRoughness());
        glBindTextureUnit(3, currentTexture->GetTextureMetalic());

        


        glUniform1f(glGetUniformLocation(programID, "Roughness"), currentTexture->GetRoughness());
        glUniform1f(glGetUniformLocation(programID, "Metalic"), currentTexture->GetMetalic());

        glUniform1i(glGetUniformLocation(programID, "IsEmissive"), currentTexture->IsEmissive());
    }

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
        1,                                // attribute
        2,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
        2,                                // attribute
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    // 4th attribute buffer : tangents
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
    glVertexAttribPointer(
        3,                                // attribute
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    // 5th attribute buffer : bitangents
    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
    glVertexAttribPointer(
        4,                                // attribute
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    
    
    // 5th attribute buffer : boneid
    glEnableVertexAttribArray(5);
    glBindBuffer(GL_ARRAY_BUFFER, jointIdbuffer);
    glVertexAttribPointer(
        5,                                // attribute
        4,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    // 7th attribute buffer : weights
    glEnableVertexAttribArray(6);
    glBindBuffer(GL_ARRAY_BUFFER, Weightbuffer);
    glVertexAttribPointer(
        6,                                // attribute
        4,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    
    
    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

    // Draw the triangles !
    glDrawElements(
        GL_TRIANGLES,      // mode
        (GLsizei)indices.size(),    // count
        GL_UNSIGNED_SHORT,   // type
        (void*)0           // element array buffer offset
    );

}
glm::vec3 Mesh::GetVertices(int index) {
    return indexed_vertices[index];
}
void Mesh::SetTexture(Texture* texture) {
    this->texture = texture;
}
std::string Mesh::GetName() {
    return name;
}
void Mesh::SetName(std::string name) {
    this->name = name;
}

std::string Mesh::GetTextureName() {
    return texture->GetName();
}
size_t Mesh::VerticiesSize() {
    return indexed_vertices.size();
}
glm::vec3 Mesh::GetVertex(int i) {
    return indexed_vertices[i];
}    
int Mesh::BindVertices(int offset, int modelMatrixIndex) {
    const int size = indexed_vertices.size();
    std::vector<glm::vec4> vertciesWithM;
    for (int i = 0; i < indexed_vertices.size(); i++) {
        vertciesWithM.push_back(glm::vec4(indexed_vertices[i], modelMatrixIndex));
    }
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, vertciesWithM.size() * sizeof(glm::vec4), &vertciesWithM[0]);
    return vertciesWithM.size() * sizeof(glm::vec4);
}
int Mesh::BindIndices(int offset) {
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, indices.size() * sizeof(unsigned short), &indices[0]);
    return indices.size() * sizeof(unsigned short);
}





