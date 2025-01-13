#version 430 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexTangent_modelspace;
layout(location = 4) in vec3 vertexBitangent_modelspace;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

out vec2 UV;
out vec3 FragPos;
out mat3 TBN; // Tangent-Bitangent-Normal matrix
out vec3 TrueNormal;


uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform mat4 P;
uniform mat3 normalMatrix3;

const int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];
uniform bool animated;

// Uniforms
uniform float time;
uniform float amplitude1 = 0.06;    // Amplitude of wave 1
uniform float frequency1 = 0.5;    // Frequency of wave 1
uniform float speed1 = 1.0;        // Speed of wave 1
uniform vec2 direction1 = vec2(1.0, 0.0); // Direction of wave 1

uniform float amplitude2 = 0.09;    // Amplitude of wave 2
uniform float frequency2 = 1.0;    // Frequency of wave 2
uniform float speed2 = 0.8;        // Speed of wave 2
uniform vec2 direction2 = vec2(0.0, 1.0); // Direction of wave 2

uniform float amplitude3 = 0.05;    // Amplitude of wave 3
uniform float frequency3 = 1.0;    // Frequency of wave 3
uniform float speed3 = 1.5;        // Speed of wave 3
uniform vec2 direction3 = vec2(1.0, 1.0); // Direction of wave 3


void main()
{	vec4 viewPos;
    vec4 worldPos;
    vec4 animationTrnasformed;

    if(animated){
        mat4 BoneTransform = finalBonesMatrices[boneIds[0]] * weights[0];
        BoneTransform     += finalBonesMatrices[boneIds[1]] * weights[1];
        BoneTransform     += finalBonesMatrices[boneIds[2]] * weights[2];
        BoneTransform     += finalBonesMatrices[boneIds[3]] * weights[3];
        animationTrnasformed =  BoneTransform * vec4(vertexPosition_modelspace,1);

        mat3 normalMatrix = transpose(inverse(mat3(V * M)));
        vec3 normal = normalize(normalMatrix * mat3(BoneTransform)  * vertexNormal_modelspace);
        vec3 tangent = normalize(normalMatrix * mat3(BoneTransform)  * vertexTangent_modelspace);
        vec3 bitangent = normalize(normalMatrix * mat3(BoneTransform)  * vertexBitangent_modelspace);

        TBN = mat3(tangent, bitangent, normal); // Construct TBN matrix for transforming the normal map
    }
    else{
        animationTrnasformed =  vec4(vertexPosition_modelspace,1.0f);
        mat3 normalMatrix = transpose(inverse(mat3(V * M)));
        vec3 normal = normalize(normalMatrix * vertexNormal_modelspace);
        vec3 tangent = normalize(normalMatrix * vertexTangent_modelspace);
        vec3 bitangent = normalize(normalMatrix * vertexBitangent_modelspace);

        TBN = mat3(tangent, bitangent, normal); // Construct TBN matrix for transforming the normal map
    }

    // Calculate wave 1 displacement
    float wave1 = amplitude1 * sin(dot(animationTrnasformed.xz, direction1) * frequency1 + time * speed1);
    
    // Calculate wave 2 displacement
    float wave2 = amplitude2 * sin(dot(animationTrnasformed.xz, direction2) * frequency2 + time * speed2);
    
    // Calculate wave 3 displacement
    float wave3 = amplitude3 * sin(dot(animationTrnasformed.xz, direction3) * frequency3 + time * speed3) - 1;

    // Sum of the waves
    float totalWave = wave1 + wave2 + pow(2.718281828459045,wave3);
    
    // Apply displacement to the vertex position
    vec3 displacedPosition = vertexPosition_modelspace;
    displacedPosition.y += totalWave;


        // Compute partial derivatives for normals
    float dy_dx = 0.0;
    float dy_dz = 0.0;

    dy_dx += amplitude1 * frequency1 * direction1.x * cos(dot(animationTrnasformed.xz, direction1) * frequency1 + time * speed1);
    dy_dz += amplitude1 * frequency1 * direction1.y * cos(dot(animationTrnasformed.xz, direction1) * frequency1 + time * speed1);

    dy_dx += amplitude2 * frequency2 * direction2.x * cos(dot(animationTrnasformed.xz, direction2) * frequency2 + time * speed2);
    dy_dz += amplitude2 * frequency2 * direction2.y * cos(dot(animationTrnasformed.xz, direction2) * frequency2 + time * speed2);

    dy_dx += amplitude3 * frequency3 * direction3.x * cos(dot(animationTrnasformed.xz, direction3) * frequency3 + time * speed3);
    dy_dz += amplitude3 * frequency3 * direction3.y * cos(dot(animationTrnasformed.xz, direction3) * frequency3 + time * speed3);

    // Construct tangent vectors
    vec3 tangentX = vec3(1.0, dy_dx, 0.0);
    vec3 tangentZ = vec3(0.0, dy_dz, 1.0);

    // Calculate the normal as the cross product of tangents
    vec3 normal = normalize(cross(tangentZ, tangentX));





    TrueNormal = vec3(M * vec4(normal,0));
    worldPos = M * vec4(displacedPosition,1);
    viewPos = V * worldPos;
    FragPos = viewPos.xyz; 
    UV = vertexUV;
    gl_Position = P * viewPos;

}