#pragma once
#include <vector>
#include <string>

#include "Engine/Core/Common.h"
#include "Engine/Core/Renderer.h"
#include "Engine/Physics/BulletPhysics.h"
#include "Engine/Core/GameObject.h"


class DecalInstance;

class Decal
{
public:
    Decal() = default;
    Decal(std::string name, Texture* texture);

    void RenderDecal(GLuint& programID);
    size_t getVerticesSize();
    std::vector<glm::vec3> getVerticies();
    glm::vec3 GetVertex(int i);
    std::string GetName();
    void AddInstace(DecalInstance* instance);
    void ClearInstace();


private:  
    std::string name;
    Texture* texture = nullptr;

    std::vector<glm::mat4> instancesModel;


    std::vector<glm::vec3> indexed_vertices = {
        // Front face
        glm::vec3(-1.0f, -1.0f,  1.0f),  // Bottom-left
        glm::vec3(1.0f, -1.0f,  1.0f),  // Bottom-right
        glm::vec3(1.0f,  1.0f,  1.0f),  // Top-right
        glm::vec3(-1.0f,  1.0f,  1.0f),  // Top-left

        // Back face
        glm::vec3(-1.0f, -1.0f, -1.0f),  // Bottom-left
        glm::vec3(1.0f, -1.0f, -1.0f),  // Bottom-right
        glm::vec3(1.0f,  1.0f, -1.0f),  // Top-right
        glm::vec3(-1.0f,  1.0f, -1.0f)   // Top-left
    };

    // Correct winding order for each face
    std::vector<unsigned short> indices = {
        // front
        0, 1, 2,
        2, 3, 0,
        // right
        1, 5, 6,
        6, 2, 1,
        // back
        7, 6, 5,
        5, 4, 7,
        // left
        4, 0, 3,
        3, 7, 4,
        // bottom
        4, 5, 1,
        1, 0, 4,
        // top
        3, 2, 6,
        6, 7, 3
    };

    // UV coordinates (matching vertex order)
    std::vector<glm::vec2> indexed_uvs = {
        // front face
        glm::vec2(0.0f, 0.0f), // 0
        glm::vec2(1.0f, 0.0f), // 1
        glm::vec2(1.0f, 1.0f), // 2
        glm::vec2(0.0f, 1.0f), // 3

        // right face
        glm::vec2(0.0f, 0.0f), // 4
        glm::vec2(1.0f, 0.0f), // 5
        glm::vec2(1.0f, 1.0f), // 6
        glm::vec2(0.0f, 1.0f), // 7

        // back face
        glm::vec2(0.0f, 0.0f), // 8
        glm::vec2(1.0f, 0.0f), // 9
        glm::vec2(1.0f, 1.0f), // 10
        glm::vec2(0.0f, 1.0f), // 11

        // left face
        glm::vec2(0.0f, 0.0f), // 12
        glm::vec2(1.0f, 0.0f), // 13
        glm::vec2(1.0f, 1.0f), // 14
        glm::vec2(0.0f, 1.0f), // 15

        // bottom face
        glm::vec2(0.0f, 0.0f), // 16
        glm::vec2(1.0f, 0.0f), // 17
        glm::vec2(1.0f, 1.0f), // 18
        glm::vec2(0.0f, 1.0f), // 19

        // top face
        glm::vec2(0.0f, 0.0f), // 20
        glm::vec2(1.0f, 0.0f), // 21
        glm::vec2(1.0f, 1.0f), // 22
        glm::vec2(0.0f, 1.0f)  // 23
    };

    // Normals (one per face)
    std::vector<glm::vec3> indexed_normals = {
        glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)
    };

    // Set tangents and bitangents to point upwards
    std::vector<glm::vec3> indexed_tangents = {
        glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)
    };

    std::vector<glm::vec3> indexed_bitangents = {
        glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)
    };


    //TODO:: Fix this so all decals are renderer in one draw call
    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;
    GLuint elementbuffer;
    GLuint VertexArrayID;
    GLuint tangentbuffer;
    GLuint bitangentbuffer;

    GLuint instanceBuffer;

};

