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

void main()
{	vec4 viewPos;

    if(animated){
        mat4 BoneTransform = finalBonesMatrices[boneIds[0]] * weights[0];
        BoneTransform     += finalBonesMatrices[boneIds[1]] * weights[1];
        BoneTransform     += finalBonesMatrices[boneIds[2]] * weights[2];
        BoneTransform     += finalBonesMatrices[boneIds[3]] * weights[3];
        viewPos = V * M * BoneTransform * vec4(vertexPosition_modelspace,1);

        mat3 normalMatrix = transpose(inverse(mat3(V * M)));
        vec3 normal = normalize(normalMatrix * mat3(BoneTransform)  * vertexNormal_modelspace);
        vec3 tangent = normalize(normalMatrix * mat3(BoneTransform)  * vertexTangent_modelspace);
        vec3 bitangent = normalize(normalMatrix * mat3(BoneTransform)  * vertexBitangent_modelspace);
    }
    else{
        viewPos = V * M * vec4(vertexPosition_modelspace,1.0f);
        mat3 normalMatrix = transpose(inverse(mat3(V * M)));
        vec3 normal = normalize(normalMatrix * vertexNormal_modelspace);
        vec3 tangent = normalize(normalMatrix * vertexTangent_modelspace);
        vec3 bitangent = normalize(normalMatrix * vertexBitangent_modelspace);
    }


    TrueNormal = vec3(M * vec4(vertexNormal_modelspace,0));
    FragPos = viewPos.xyz; 
    UV = vertexUV;
    gl_Position = P * viewPos;

}