#version 430 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexTangent_modelspace;
layout(location = 4) in vec3 vertexBitangent_modelspace;
//transparent

out vec2 UV;
out mat3 TBN; // Tangent-Bitangent-Normal matrix
out vec4 FragPos;
out vec3 FragN;
out vec4 viewFragPos;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform mat4 P;
uniform mat3 normalMatrix3;


void main()
{	
    FragPos = M * vec4(vertexPosition_modelspace, 1.0);
    UV = vertexUV;

        // Transform normals, tangents, and bitangents to world space
    mat3 normalMatrix = transpose(inverse(mat3(M)));
    vec3 normal = normalize(normalMatrix * vertexNormal_modelspace);
    vec3 tangent = normalize(normalMatrix * vertexTangent_modelspace);
    vec3 bitangent = normalize(normalMatrix * vertexBitangent_modelspace);

    TBN = mat3(tangent, bitangent, normal); // Construct TBN matrix for transforming the normal map
    FragN = normalize(M * vec4(vertexNormal_modelspace,0)).xyz;
    gl_Position = P * V * FragPos;
    viewFragPos = V * FragPos;
}