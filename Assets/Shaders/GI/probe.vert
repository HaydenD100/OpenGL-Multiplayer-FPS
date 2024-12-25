#version 430 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexTangent_modelspace;
layout(location = 4) in vec3 vertexBitangent_modelspace;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

out vec2 UV;
out mat3 TBN; // Tangent-Bitangent-Normal matrix
out vec3 FragPos;
out vec3 Normal;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

uniform mat3 normalMatrix3;

const int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];
uniform bool animated;

void main()
{	vec4 WorldPos;

    if(animated){
        mat4 BoneTransform = finalBonesMatrices[boneIds[0]] * weights[0];
        BoneTransform     += finalBonesMatrices[boneIds[1]] * weights[1];
        BoneTransform     += finalBonesMatrices[boneIds[2]] * weights[2];
        BoneTransform     += finalBonesMatrices[boneIds[3]] * weights[3];
        WorldPos = M * BoneTransform * vec4(vertexPosition_modelspace,1);

        mat3 normalMatrix = transpose(inverse(mat3(M)));
        vec3 normal = normalize(mat3(BoneTransform)  * vertexNormal_modelspace);
        vec3 tangent = normalize(mat3(BoneTransform)  * vertexTangent_modelspace);
        vec3 bitangent = normalize(mat3(BoneTransform)  * vertexBitangent_modelspace);

        TBN = mat3(tangent, bitangent, normal); // Construct TBN matrix for transforming the normal map
    }
    else{
        WorldPos = M * vec4(vertexPosition_modelspace,1.0f);
        mat3 normalMatrix = transpose(inverse(mat3(M)));
        vec3 normal = normalize(normalMatrix * vertexNormal_modelspace);
        vec3 tangent = normalize(normalMatrix * vertexTangent_modelspace);
        vec3 bitangent = normalize(normalMatrix * vertexBitangent_modelspace);

        TBN = mat3(tangent, bitangent, normal); // Construct TBN matrix for transforming the normal map
    }
    UV = vertexUV;
    Normal = vertexNormal_modelspace;
    FragPos = vec3(WorldPos);
    gl_Position = P * V * WorldPos;
}