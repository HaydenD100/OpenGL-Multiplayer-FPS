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

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform mat4 P;
uniform mat3 normalMatrix3;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{	vec4 viewPos;

    if(boneIds.x != -1){
        vec4 totalPosition = vec4(0.0f);
        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
        {
            if(boneIds[i] == -1) 
                continue;
            if(boneIds[i] >=MAX_BONES) 
            {
                totalPosition = vec4(vertexPosition_modelspace,1.0f);
                break;
            }
            vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(vertexPosition_modelspace,1.0f);
            totalPosition += localPosition * weights[i];
            vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * vertexNormal_modelspace;
        }
        viewPos = V * M * totalPosition;
    }
    else{
        viewPos = V * M * vec4(vertexPosition_modelspace,1.0f);
    }
    FragPos = viewPos.xyz; 
    UV = vertexUV;
    mat3 normalMatrix = transpose(inverse(mat3(V * M)));
    vec3 normal = normalize(normalMatrix * vertexNormal_modelspace);
    vec3 tangent = normalize(normalMatrix * vertexTangent_modelspace);
    vec3 bitangent = normalize(normalMatrix * vertexBitangent_modelspace);

    TBN = mat3(tangent, bitangent, normal); // Construct TBN matrix for transforming the normal map

    
    gl_Position = P * viewPos;
}