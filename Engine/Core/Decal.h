#pragma once
#include <vector>
#include <string>

#include "Engine/Core/Common.h"
#include "Engine/Core/Renderer.h"
#include "Engine/Physics/BulletPhysics.h"
#include "Engine/Core/GameObject.h"

class Decal
{
public:
    Decal() = default;
    Decal(glm::vec3 position, glm::vec3 normal, glm::vec3 scale, Texture* texture, GameObject* Parent);
    glm::mat4 GetModel();
    glm::vec3 GetNormal();
    bool CheckParentIsNull();
    void RenderDecal(GLuint& programID);

    glm::vec3 GetPosition() const {
        return transform.position;
    }
private:    
    Texture* texture = nullptr;
    glm::vec3 normal = glm::vec3(0, 0, 0);
    GameObject* parent = nullptr;
    Transform transform;

    std::vector<unsigned short> indices = { 0, 1, 2, 0, 3, 1 };
    std::vector<glm::vec3> indexed_vertices = { glm::vec3(1.0, 0.0,1.0),glm::vec3(-1.0,0.0,-1.0),glm::vec3(-1.0, 0.0,1.0),glm::vec3(1.0,0.0,-1.0) };
    std::vector<glm::vec2> indexed_uvs = { glm::vec2(1.0,-0.0),glm::vec2(0.0,-1.0),glm::vec2(0.0,-0.0),glm::vec2(1.0,-1.0) };
    std::vector<glm::vec3> indexed_normals = { glm::vec3(-0.0,1.0,-0.0),glm::vec3(-0.0,1.0,-0.0),glm::vec3(-0.0,1.0,-0.0),glm::vec3(-0.0,1.0,-0.0) };
    std::vector<glm::vec3> indexed_tangents = { glm::vec3(-0.0,1.0,-0.0),glm::vec3(-0.0,1.0,-0.0),glm::vec3(-0.0,1.0,-0.0),glm::vec3(-0.0,1.0,-0.0) };
    std::vector<glm::vec3> indexed_bitangents = { glm::vec3(-0.0,1.0,-0.0),glm::vec3(-0.0,1.0,-0.0),glm::vec3(-0.0,1.0,-0.0),glm::vec3(-0.0,1.0,-0.0) };


    //TODO:: Fix this so all decals are renderer in one draw call
    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;
    GLuint elementbuffer;
    GLuint VertexArrayID;
    GLuint tangentbuffer;
    GLuint bitangentbuffer;
};
