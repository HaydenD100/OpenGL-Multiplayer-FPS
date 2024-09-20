#include "Common.h"
#include "Engine/Loaders/Loader.hpp"
#include "Engine/Loaders/vboindexer.h"
#include "Engine/Loaders/stb_image.h"
#include "Engine/Core/AssetManager.h"

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
