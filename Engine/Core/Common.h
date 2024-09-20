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
#include "Engine/Core/Model.h"



#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define SCREENWIDTH 1280
#define SCREENHEIGHT 720
#define RATIO 16.0f/9.0f
#define WINDOWTITILE "Engine 2.0"

//Rendering stuff
#define MAXDECALS 75





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

btVector3 glmToBtVector3(const glm::vec3& vec);

glm::vec3 btToGlmVector3(const btVector3& vec);

glm::vec3 btQuatToGLMVec(const btQuaternion& quat);

glm::mat4 btTransformToGlmMatrix(const btTransform& transform);

glm::mat4 btScalar2mat4(btScalar* matrix);

glm::mat4 worldToLocal(btVector3 position, btVector3 rotation);

glm::vec3 ConvertBlenderPositionToOpenGL(const glm::vec3& blenderPosition);

glm::quat ConvertBlenderQuaternionToOpenGL(const glm::quat& blenderQuat);

glm::vec3 ConvertBlenderScaleToOpenGL(const glm::vec3& blenderScale);

float getRandomFloat(float min, float max);

glm::vec3 randomVector(float maxAngle);

glm::vec3 offsetRayWithinAngle(const glm::vec3& rayDir, float maxOffsetAngle);

float finlerpTo(float start, float end, float deltaTime, float interpSeed);
