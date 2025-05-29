#include "Common.h"
#include "Engine/Loaders/Loader.hpp"
#include "Engine/Loaders/vboindexer.h"
#include "Engine/Loaders/stb_image.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Core/Camera.h"
#include "Engine/Core/Common/Header.h"


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
double round_up(double value, int decimal_places) {
    const double multiplier = std::pow(10.0, decimal_places);
    return std::ceil(value * multiplier) / multiplier;
}

void checkOpenGLError(const char* functionName) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error in " << functionName << ": " << gluErrorString(error) << std::endl;
    }
}

void checkOpenGLError() {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error " << gluErrorString(error) << std::endl;
    }
}

glm::vec3 ScreenPointToRayFunc(
    float screenX, float screenY,      // Pixel coordinates
    float screenWidth, float screenHeight,  // Screen size
    const glm::mat4& viewMatrix,       // View matrix
    const glm::mat4& projectionMatrix  // Projection matrix
) {
    // Convert screen position to normalized device coordinates (NDC)
    float ndcX = (2.0f * screenX) / screenWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * screenY) / screenHeight; // Flip Y for OpenGL
    float ndcZ = -1.0f; // Start the ray at near plane
    float farZ = 1.0f;  // End the ray at far plane

    // Convert NDC to world space (unproject)
    glm::mat4 invVP = glm::inverse(projectionMatrix * viewMatrix);
    glm::vec4 nearPoint = invVP * glm::vec4(ndcX, ndcY, ndcZ, 1.0f);
    glm::vec4 farPoint = invVP * glm::vec4(ndcX, ndcY, farZ, 1.0f);

    // Perspective divide (convert from homogeneous coordinates)
    nearPoint /= nearPoint.w;
    farPoint /= farPoint.w;

    // Compute ray direction (normalized)
    glm::vec3 rayOrigin = glm::vec3(nearPoint);
    glm::vec3 rayDirection = glm::normalize(glm::vec3(farPoint) - rayOrigin);

    return rayDirection;
}


std::vector<GLfloat> generateGaussianNoise(int width, int height) {
    std::vector<GLfloat> pixels(width * height * 4); // Now 4 channels (RGBA)

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<GLfloat> dist(0.0f, 1.0f);

    for (int i = 0; i < width * height; ++i) {
        // Generate two pairs of (u, v) for two Box-Muller transforms
        float u1 = 1.0f - dist(gen); // Avoid log(0)
        float v1 = 1.0f - dist(gen);
        float u2 = 1.0f - dist(gen);
        float v2 = 1.0f - dist(gen);

        // First Box-Muller transform (Red/Green channels)
        float radius1 = std::sqrt(-2.0f * std::log(u1));
        float angle1 = 2.0f * ENGINE_PI * v1;
        float z0 = radius1 * std::cos(angle1);
        float z1 = radius1 * std::sin(angle1);

        // Second Box-Muller transform (Blue/Alpha channels)
        float radius2 = std::sqrt(-2.0f * std::log(u2));
        float angle2 = 2.0f * ENGINE_PI * v2;
        float z2 = radius2 * std::cos(angle2);
        float z3 = radius2 * std::sin(angle2);

        // Assign to all four channels
        pixels[i * 4 + 0] = z0; // Red
        pixels[i * 4 + 1] = z1; // Green
        pixels[i * 4 + 2] = z2; // Blue
        pixels[i * 4 + 3] = z3; // Alpha
    }

    return pixels;
}


std::vector<GLfloat> generatePhillipsDistribution(int Nx, int Nz, glm::vec2 size, float amplitude,
    float max_l, float L, glm::vec2 wind_dir) {
    std::vector<GLfloat> pixels(Nx * Nz * 4); // RGBA channels

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<GLfloat> dist(0.0f, 1.0f);

    glm::vec2 mod = glm::vec2(2.0f * ENGINE_PI) / size;

    for (int z = 0; z < Nz; ++z) {
        for (int x = 0; x < Nx; ++x) {
            // Calculate aliased wave vector
            glm::vec2 k = mod * glm::vec2(alias(x, Nx), alias(z, Nz));

            // Compute Phillips spectrum
            float phillips_val = phillips(k, max_l, L, wind_dir);
            float scale = amplitude * sqrt(0.5f * phillips_val);

            // Generate Gaussian pairs using Box-Muller
            float u1 = 1.0f - dist(gen);
            float v1 = dist(gen);
            float radius1 = sqrt(-2.0f * log(u1));
            float angle1 = 2.0f * ENGINE_PI * v1;
            float z0 = radius1 * cos(angle1) * scale;
            float z1 = radius1 * sin(angle1) * scale;

            float u2 = 1.0f - dist(gen);
            float v2 = dist(gen);
            float radius2 = sqrt(-2.0f * log(u2));
            float angle2 = 2.0f * ENGINE_PI * v2;
            float z2 = radius2 * cos(angle2) * scale;
            float z3 = radius2 * sin(angle2) * scale;

            // Store in RGBA channels
            int idx = (z * Nx + x) * 4;
            pixels[idx + 0] = z0; // Red
            pixels[idx + 1] = z1; // Green
            pixels[idx + 2] = z2; // Blue
            pixels[idx + 3] = z3; // Alpha
        }
    }

    return pixels;
}

// Helper function for aliasing indices
int alias(int index, int N) {
    return index < N / 2 ? index : index - N;
}

// Phillips spectrum implementation
float phillips(glm::vec2 k, float max_l, float L, glm::vec2 wind_dir) {
    float k_len = glm::length(k);
    if (k_len == 0.0f) return 0.0f;

    float kL = k_len * L;
    glm::vec2 k_dir = glm::normalize(k);
    float kw = glm::dot(k_dir, wind_dir);

    return pow(fabs(kw), 1.0f) *
        exp(-k_len * k_len * max_l * max_l) *
        exp(-1.0f / (kL * kL)) *
        pow(k_len, -4.0f);
}
std::vector<glm::vec3> generate_random_directions(int count) {
    assert(count > 0 && "Direction count must be positive");
    std::vector<glm::vec3> directions;
    directions.reserve(count);

    // Random number setup
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> dist(0.0f, 1.0f);

    for (int i = 0; i < count; ++i) {
        // Generate random components
        float x = dist(gen);
        float y = dist(gen);
        float z = dist(gen);

        // Normalize and store
        directions.emplace_back(glm::normalize(glm::vec3(x, y, z)));
    }

    return directions;
}

std::string generateRandomString(size_t length) {
    const std::string chars =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    std::random_device rd;                      // Seed
    std::mt19937 generator(rd());               // Mersenne Twister engine
    std::uniform_int_distribution<> dist(0, chars.size() - 1);

    std::string result;
    result.reserve(length);

    for (size_t i = 0; i < length; ++i) {
        result += chars[dist(generator)];
    }

    return result;
}