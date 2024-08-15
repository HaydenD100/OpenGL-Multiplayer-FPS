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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#define SCREENWIDTH 1280
#define SCREENHEIGHT 720
#define RATIO 16.0f/9.0f
#define WINDOWTITILE "Engine 2.0"


struct Transform{
    glm::vec3 position = glm::vec3(0);
    glm::vec3 rotation = glm::vec3(0);
    glm::vec3 scale = glm::vec3(1);

    glm::vec3 getPosition() { return position; }
    glm::vec3 getRotation() { return rotation; }
    glm::vec3 getScale() { return scale; }

    glm::mat4 to_mat4() {
        glm::mat4 m = glm::translate(glm::mat4(1), position);
        m *= glm::mat4_cast(glm::quat(rotation));
        m = glm::scale(m, scale);
        return m;
    }
};



struct Mesh {
    Mesh(const char* path);
    Mesh(std::vector<glm::vec3> vertices,
    std::vector<glm::vec3> normals,
    std::vector<glm::vec2> UV,
    std::vector<unsigned short> indices,
    std::vector<glm::vec3> tangets,
    std::vector<glm::vec3> bitTangents);

    void Render();
    glm::vec3 GetVertices(int index);

    std::vector<unsigned short> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;
    std::vector<glm::vec3> indexed_tangents;
    std::vector<glm::vec3> indexed_bitangents;


private:
    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;
    GLuint elementbuffer;
    GLuint tangentbuffer;
    GLuint bitangentbuffer;

};

class Model {
public:
    Model() = default;
    Model(Mesh mesh, Texture* texture);
    Model(const char* path, Texture* texture);
    Model(const char* path, const char* collisonShapePath, Texture* texture);
    void AddMesh(Mesh mesh);
    void SetMesh(int mesh);
    Mesh* GetCurrentMesh();
    std::vector<Mesh>* GetAllMeshes();
    Mesh* GetMesh(int i);
    size_t GetMeshSize();
    const char* GetTextureName();
    void RenderModel(GLuint& programID);
    void RenderAllMeshes(bool state);
    bool RenderAll();

    std::vector<glm::vec3> GetColliderShapeVerticies();
    size_t GetColliderShapeVerticiesSize();

private:
    bool renderAllMeshes = true;
    std::vector<Mesh> meshes;
    int currentMesh = 0;
    Texture* texture = nullptr;

    //for collison shape
    std::vector<glm::vec3> collison_shape_vertices;

    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};

btVector3 glmToBtVector3(const glm::vec3& vec);

glm::vec3 btToGlmVector3(const btVector3& vec);

glm::vec3 btQuatToGLMVec(const btQuaternion& quat);

glm::mat4 btTransformToGlmMatrix(const btTransform& transform);

glm::mat4 btScalar2mat4(btScalar* matrix);

glm::mat4 worldToLocal(btVector3 position, btVector3 rotation);

// TODO: I do want to add a mesh instead of storing all the obj info in GameObject this will also make it easy to allow multiple GameObjects to use the same mesh without loading a model each time
