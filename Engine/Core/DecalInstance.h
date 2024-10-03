#pragma once
#include <vector>
#include <string>

#include "Engine/Core/Common.h"
#include "Engine/Core/Renderer.h"

#include "Engine/Core/GameObject.h"

class Decal;

class DecalInstance {
public:
    DecalInstance() = default;
    DecalInstance(glm::vec3 position, glm::vec3 normal, Decal* decal, GameObject* Parent);

    Transform getTransform();
    bool CheckParentIsNull();

    AABB* GetAABB();
    glm::mat4 GetModel();
    glm::vec3 GetNormal();
    glm::vec3 GetScale();
    Decal* GetDecal();

    glm::vec3 GetPosition() const {
        return transform.position;
    }
private:
    Decal* decal;
    AABB aabb;
    AABB generateAABB();
    glm::vec3 normal = glm::vec3(0, 0, 0);
    GameObject* parent = nullptr;
    Transform transform;


};
