#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "bullet/btBulletDynamicsCommon.h"
#include <array>     // For std::array
#include <glm/vec3.hpp>  // For glm::vec3

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

//Rendering stuff
#define MAXDECALS 3000





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
	glm::mat4 to_mat4() const {
		glm::mat4 m = glm::translate(glm::mat4(1), position);
		m *= glm::mat4_cast(glm::quat(rotation));
		m = glm::scale(m, scale);
		return m;
	}

	glm::vec3 getRight() const
	{
		return to_mat4()[0];
	}


	glm::vec3 getUp() const
	{
		return to_mat4()[1];
	}

	glm::vec3 getBackward() const
	{
		return to_mat4()[2];
	}

	glm::vec3 getForward() const
	{
		return -to_mat4()[2];
	}
};

//LearnOpenGl code
struct Plane
{
    glm::vec3 normal = { 0.f, 1.f, 0.f }; // unit vector
    float     distance = 0.f;        // Distance with origin

    Plane() = default;

    Plane(const glm::vec3& p1, const glm::vec3& norm)
        : normal(glm::normalize(norm)),
        distance(glm::dot(normal, p1))
    {}

    float getSignedDistanceToPlane(const glm::vec3& point) const
    {
        return glm::dot(normal, point) - distance;
    }
};
struct Frustum
{
    Plane topFace;
    Plane bottomFace;

    Plane rightFace;
    Plane leftFace;

    Plane farFace;
    Plane nearFace;
};
struct BoundingVolume
{
    virtual bool isOnFrustum(const Frustum& camFrustum, const Transform& transform) const = 0;
    virtual bool isOnOrForwardPlane(const Plane& plane) const = 0;

    bool isOnFrustum(const Frustum& camFrustum) const
    {
        return (isOnOrForwardPlane(camFrustum.leftFace) &&
            isOnOrForwardPlane(camFrustum.rightFace) &&
            isOnOrForwardPlane(camFrustum.topFace) &&
            isOnOrForwardPlane(camFrustum.bottomFace) &&
            isOnOrForwardPlane(camFrustum.nearFace) &&
            isOnOrForwardPlane(camFrustum.farFace));
    }
};

struct AABB : public BoundingVolume
{
    glm::vec3 center{ 0.f, 0.f, 0.f };
    glm::vec3 extents{ 0.f, 0.f, 0.f };

    AABB() = default;
    AABB(const glm::vec3& min, const glm::vec3& max);
    AABB(const glm::vec3& inCenter, float iI, float iJ, float iK);

    std::array<glm::vec3, 8> getVertice() const;

    bool isOnOrForwardPlane(const Plane& plane) const final;
    bool isOnFrustum(const Frustum& camFrustum, const Transform& transform) const final;
};




Frustum createFrustumFromCamera(float aspect, float fovY, float zNear, float zFar);


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

glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);

glm::vec3 GetGLMVec(const aiVector3D& vec);

glm::quat GetGLMQuat(const aiQuaternion& quat);

double round_up(double value, int decimal_places);



