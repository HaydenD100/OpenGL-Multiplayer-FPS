#include "Common.h"



btVector3 glmToBtVector3(const glm::vec3& vec) {
    return btVector3(vec.x, vec.y, vec.z);
}
glm::vec3 btToGlmVector3(const btVector3& vec) {
    return glm::vec3(vec.getX(), vec.getY(), vec.getZ());
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

