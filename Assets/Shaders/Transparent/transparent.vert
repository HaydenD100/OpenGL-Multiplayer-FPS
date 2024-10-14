#version 430 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexTangent_modelspace;
layout(location = 4) in vec3 vertexBitangent_modelspace;

out vec2 UV;
out mat3 TBN; // Tangent-Bitangent-Normal matrix
out vec4 FragPos;

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
    vec3 normal = normalize(vec3(M * vec4(vertexNormal_modelspace, 0.0))); // Normal transformed to world space
    vec3 tangent = normalize(vec3(M * vec4(vertexTangent_modelspace, 0.0))); // Tangent transformed to world space
    vec3 bitangent = normalize(vec3(M * vec4(vertexBitangent_modelspace, 0.0))); // Bitangent transformed to world space


    TBN = mat3(tangent, bitangent, normal); // Construct TBN matrix for transforming the normal map
    gl_Position = P * V * FragPos;
}