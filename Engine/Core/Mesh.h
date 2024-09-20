#pragma once
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
#include "Engine/Loaders/vboindexer.h"
struct Mesh {
    Mesh(const char* path);
    Mesh(std::vector<glm::vec3> vertices,
        std::vector<glm::vec3> normals,
        std::vector<glm::vec2> UV,
        std::vector<unsigned short> indices,
        std::vector<glm::vec3> tangets,
        std::vector<glm::vec3> bitTangents);

    void Render(GLuint programID);
    glm::vec3 GetVertices(int index);
    const char* GetTextureName();
    std::string GetName();
    void SetName(std::string name);
    void SetTexture(Texture* texture);


    std::vector<unsigned short> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;
    std::vector<glm::vec3> indexed_tangents;
    std::vector<glm::vec3> indexed_bitangents;


private:
    std::string name;
    Texture* texture = nullptr;
    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;
    GLuint elementbuffer;
    GLuint tangentbuffer;
    GLuint bitangentbuffer;

};