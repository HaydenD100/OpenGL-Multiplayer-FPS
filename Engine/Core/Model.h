#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "bullet/btBulletDynamicsCommon.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Engine/Core/Texture.h"
#include "Engine/Core/Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Engine/Core/Common.h"


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
    void RenderModel(GLuint& programID);
    void RenderAllMeshes(bool state);
    bool RenderAll();

    std::vector<glm::vec3> GetColliderShapeVerticies();
    size_t GetColliderShapeVerticiesSize();


private:
    AABB aabb;
    bool renderAllMeshes = true;
    std::vector<Mesh> meshes;
    int currentMesh = 0;

    //for collison shape
    std::vector<glm::vec3> collison_shape_vertices;
    AABB generateAABB();


    void processNode(aiNode* node, const aiScene* scene, Texture* texture);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};