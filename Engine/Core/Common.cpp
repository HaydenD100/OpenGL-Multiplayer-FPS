#include "Common.h"
#include "Engine/Loaders/Loader.hpp"
#include "Engine/Loaders/vboindexer.h"
#include "Engine/Loaders/stb_image.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Core/Camera.h"


#include <random>
#include <iostream>
#include <glm/gtx/rotate_vector.hpp>



btVector3 glmToBtVector3(const glm::vec3& vec) {
    return btVector3(vec.x, vec.y, vec.z);
}

float getRandomFloat(float min, float max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return static_cast<float>(dis(gen));
}

glm::vec3 offsetRayWithinAngle(const glm::vec3& rayDir, float maxOffsetAngle) {
    // Generate a random angle within the range [0, maxOffsetAngle] (in radians)
    float offsetAngle = getRandomFloat(0.0f, maxOffsetAngle);

    // Generate a random azimuthal angle between 0 and 2 * pi for full rotation
    float azimuthalAngle = getRandomFloat(0.0f, glm::two_pi<float>());

    // Create a random axis perpendicular to the ray direction using azimuthal angle
    glm::vec3 randomAxis = glm::normalize(glm::cross(rayDir, glm::vec3(cos(azimuthalAngle), sin(azimuthalAngle), 0.0f)));

    // Rotate the ray direction by the random angle around the random axis
    glm::vec3 offsetDir = glm::rotate(rayDir, offsetAngle, randomAxis);

    return glm::normalize(offsetDir); // Return the normalized direction
}

// Interpolates from 'start' to 'end' based on a speed factor 'interpSeed', clamping the result at the target.
float finlerpTo(float start, float end, float deltaTime, float interpSeed) {
    // Calculate the maximum step we can take towards the target using the speed (interpSeed)
    float step = interpSeed * deltaTime;

    // Determine the direction of interpolation (positive or negative)
    if (start < end) {
        // Move towards the target with the speed but clamp it to the maximum possible value (end)
        return std::min(start + step, end);
    }
    else {
        // Move towards the target in reverse if start > end, clamping to the minimum (end)
        return std::max(start - step, end);
    }
}


// Function to generate a random vector with an angle from 0 to maxAngle
glm::vec3 randomVector(float maxAngle) {
    // Generate a random angle between 0 and maxAngle (in radians)
    float theta = getRandomFloat(0.0f, maxAngle);
    // Generate a random azimuthal angle between 0 and 2 * pi
    float phi = getRandomFloat(0.0f, glm::two_pi<float>());

    // Convert spherical coordinates to Cartesian coordinates
    float x = sin(theta) * cos(phi);
    float y = sin(theta) * sin(phi);
    float z = cos(theta);

    // Return the unit vector (it will have a magnitude of 1)
    return glm::normalize(glm::vec3(x, y, z));
}

glm::vec3 btToGlmVector3(const btVector3& vec) {
    return glm::vec3(vec.getX(), vec.getY(), vec.getZ());
}
glm::vec3 ConvertBlenderPositionToOpenGL(const glm::vec3& blenderPosition) {
    // Convert from Blender's (x, y, z) to OpenGL's (x, z, -y)
    return glm::vec3(blenderPosition.x, blenderPosition.z, -blenderPosition.y);
}

glm::quat ConvertBlenderQuaternionToOpenGL(const glm::quat& blenderQuat) {
    // Convert from Blender's (w, x, y, z) to OpenGL's (w, x, z, -y)
    return glm::quat(blenderQuat.w, blenderQuat.x, blenderQuat.z, -blenderQuat.y);
}

glm::vec3 ConvertBlenderScaleToOpenGL(const glm::vec3& blenderScale) {
    // Convert from Blender's (x, y, z) to OpenGL's (x, z, y)
    return glm::vec3(blenderScale.x, blenderScale.z, blenderScale.y);
}

glm::vec3 btQuatToGLMVec(const btQuaternion& quat) {
    // Convert btQuaternion to glm::quat
    glm::quat glmQuat(quat.getW(), quat.getX(), quat.getY(), quat.getZ());

    // Convert glm::quat to glm::mat4
    glm::mat4 rotationMatrix = glm::mat4_cast(glmQuat);

    // Extract Euler angles from glm::mat4
    glm::vec3 eulerAngles = glm::eulerAngles(glmQuat);

    return eulerAngles;
}

glm::mat4 btTransformToGlmMatrix(const btTransform& transform) {
    // Extract position and rotation from Bullet transform
    btVector3 position = transform.getOrigin();
    btQuaternion rotation = transform.getRotation();

    // Convert Bullet quaternion to glm::quat
    glm::quat glmRotation(rotation.w(), rotation.x(), rotation.y(), rotation.z());

    // Create glm::mat4 from position and rotation
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position.x(), position.y(), position.z()));
    model = model * glm::mat4_cast(glmRotation);

    return model;
}

glm::mat4 btScalar2mat4(btScalar* matrix) {
    return glm::mat4(
        matrix[0], matrix[1], matrix[2], matrix[3],
        matrix[4], matrix[5], matrix[6], matrix[7],
        matrix[8], matrix[9], matrix[10], matrix[11],
        matrix[12], matrix[13], matrix[14], matrix[15]);
}

glm::mat4 worldToLocal(btVector3 position, btVector3 rotation) {
    glm::mat4 m = glm::translate(glm::mat4(1), btToGlmVector3(position));
    m *= glm::mat4_cast(glm::quat(btToGlmVector3(rotation)));
    m = glm::scale(m, glm::vec3(1, 1, 1));
    return m;
}

Frustum createFrustumFromCamera(float aspect, float fovY, float zNear, float zFar)
{
    Frustum frustum;
    const float halfVSide = zFar * tanf(fovY * .5f);
    const float halfHSide = halfVSide * aspect;
    const glm::vec3 frontMultFar = zFar * Camera::GetDirection();

    frustum.nearFace = { Camera::GetPosition() + zNear * Camera::GetDirection(), Camera::GetDirection()};
    frustum.farFace = { Camera::GetPosition() + frontMultFar, -Camera::GetDirection() };
    frustum.rightFace = { Camera::GetPosition(), glm::cross(frontMultFar - Camera::GetRight() * halfHSide, Camera::GetUp()) };
    frustum.leftFace = { Camera::GetPosition(), glm::cross(Camera::GetUp(), frontMultFar + Camera::GetRight() * halfHSide) };
    frustum.topFace = { Camera::GetPosition(), glm::cross(Camera::GetRight(), frontMultFar - Camera::GetUp() * halfVSide) };
    frustum.bottomFace = { Camera::GetPosition(), glm::cross(frontMultFar + Camera::GetUp() * halfVSide, Camera::GetRight()) };
    return frustum;
}

AABB::AABB(const glm::vec3& min, const glm::vec3& max)
    : center{ (max + min) * 0.5f }, extents{ max.x - center.x, max.y - center.y, max.z - center.z }
{
}

AABB::AABB(const glm::vec3& inCenter, float iI, float iJ, float iK)
    : center{ inCenter }, extents{ iI, iJ, iK }
{
}

std::array<glm::vec3, 8> AABB::getVertice() const
{
    std::array<glm::vec3, 8> vertice;
    vertice[0] = { center.x - extents.x, center.y - extents.y, center.z - extents.z };
    vertice[1] = { center.x + extents.x, center.y - extents.y, center.z - extents.z };
    vertice[2] = { center.x - extents.x, center.y + extents.y, center.z - extents.z };
    vertice[3] = { center.x + extents.x, center.y + extents.y, center.z - extents.z };
    vertice[4] = { center.x - extents.x, center.y - extents.y, center.z + extents.z };
    vertice[5] = { center.x + extents.x, center.y - extents.y, center.z + extents.z };
    vertice[6] = { center.x - extents.x, center.y + extents.y, center.z + extents.z };
    vertice[7] = { center.x + extents.x, center.y + extents.y, center.z + extents.z };
    return vertice;
}

bool AABB::isOnOrForwardPlane(const Plane& plane) const
{
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    const float r = extents.x * std::abs(plane.normal.x) +
        extents.y * std::abs(plane.normal.y) +
        extents.z * std::abs(plane.normal.z);

    return -r <= plane.getSignedDistanceToPlane(center);
}

bool AABB::isOnFrustum(const Frustum& camFrustum, const Transform& transform) const
{
    // Get global scale thanks to our transform
    const glm::vec3 globalCenter{ transform.to_mat4() * glm::vec4(center, 1.f) };

    // Scaled orientation
    const glm::vec3 right = transform.getRight() * extents.x;
    const glm::vec3 up = transform.getUp() * extents.y;
    const glm::vec3 forward = transform.getForward() * extents.z;

    const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

    const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

    const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

    const AABB globalAABB(globalCenter, newIi, newIj, newIk);

    return (globalAABB.isOnOrForwardPlane(camFrustum.leftFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.rightFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.topFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.bottomFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.nearFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.farFace));
}


glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from) {
    glm::mat4 to;
    // Assimp uses row-major, while GLM uses column-major, so we need to transpose.
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;

    return to;
}

glm::vec3 GetGLMVec(const aiVector3D& vec) {
    return glm::vec3(vec.x, vec.y, vec.z);
}
glm::quat GetGLMQuat(const aiQuaternion& quat) {
    return glm::quat(quat.w, quat.x, quat.y, quat.z); // glm::quat constructor uses (w, x, y, z)
}
