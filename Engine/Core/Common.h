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


#define SCREENWIDTH 1024
#define SCREENHEIGHT 768
#define WINDOWTITILE "Engine 2.0"


struct Transform {
    glm::vec3 position = glm::vec3(0);
    glm::vec3 rotation = glm::vec3(0);
    glm::vec3 scale = glm::vec3(1);

    glm::vec3 getPosition() {
        return position;
    }
    glm::vec3 getRotation() {
        return rotation;
    }
    glm::vec3 getScale() {
        return scale;
    }

    glm::mat4 to_mat4() {
        glm::mat4 m = glm::translate(glm::mat4(1), position);
        m *= glm::mat4_cast(glm::quat(rotation));
        m = glm::scale(m, scale);
        return m;
    };
};


btVector3 glmToBtVector3(const glm::vec3& vec);
glm::vec3 btToGlmVector3(const btVector3& vec);
glm::vec3 btQuatToGLMVec(const btQuaternion& quat);
glm::mat4 btTransformToGlmMatrix(const btTransform& transform);
glm::mat4 btScalar2mat4(btScalar* matrix);
glm::mat4 worldToLocal(btVector3 position, btVector3 rotation);
// TODO: I do want to add a mesh instead of storing all the obj info in GameObject this will also make it easy to allow multiple GameObjects to use the same mesh without loading a model each time
