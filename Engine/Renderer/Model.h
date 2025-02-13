#pragma once

#include <map>
#include <vector>
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Animation/Bone.h"
#include "Engine/Core/Common/RenderCommon.h"


struct BoneInfo {
    int id;
    glm::mat4 offset;

};

class Model {
public:
    Model() = default;
    AABB* GetAABB();

    Model(Mesh mesh, Texture* texture);
    Model(const char* path, Texture* texture);
    Model(const char* path, const char* collisonShapePath, Texture* texture);
    void AddMesh(Mesh mesh);
    void SetMesh(int mesh);
    Mesh* GetCurrentMesh();
    Mesh* GetMeshByName(std::string name);
    std::vector<Mesh>* GetAllMeshes();
    Mesh* GetMesh(int i);
    size_t GetMeshSize();
    void RenderModel(GLuint programID);
    void RenderAllMeshes(bool state);
    bool RenderAll();
    std::string GetName();

    int VertexInfoBind(int offset, int modelMatrixIndex);
    int IndicesInfoBind(int offset);


    std::vector<glm::vec3> GetColliderShapeVerticies();
    size_t GetColliderShapeVerticiesSize();

    std::vector<glm::vec3> GetVerticies();
    std::vector<glm::vec4> GetVerticiesPadded();


    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCount() { return m_BoneCounter; }

private:
    AABB aabb;
    bool renderAllMeshes = true;
    std::vector<Mesh> meshes;
    int currentMesh = 0;
    std::string name;

    std::map<std::string, BoneInfo> m_BoneInfoMap; //
    int m_BoneCounter = 0;

    //for collison shape
    std::vector<glm::vec3> collison_shape_vertices;
    AABB generateAABB();


    void processNode(aiNode* node, const aiScene* scene, Texture* texture);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};