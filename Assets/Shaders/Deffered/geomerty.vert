#version 430 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexTangent_modelspace;
layout(location = 4) in vec3 vertexBitangent_modelspace;

out vec2 UV;
out vec3 FragPos;
out mat3 TBN; // Tangent-Bitangent-Normal matrix

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform mat4 P;
uniform mat3 normalMatrix3;


void main()
{	
    vec4 viewPos = V * M * vec4(vertexPosition_modelspace, 1.0);
    FragPos = viewPos.xyz; 
    UV = vertexUV;
    mat3 normalMatrix = transpose(inverse(mat3(V * M)));
    vec3 normal = normalize(normalMatrix * vertexNormal_modelspace);
    vec3 tangent = normalize(normalMatrix * vertexTangent_modelspace);
    vec3 bitangent = normalize(normalMatrix * vertexBitangent_modelspace);

    TBN = mat3(tangent, bitangent, normal); // Construct TBN matrix for transforming the normal map

    
    gl_Position = P * viewPos;
}