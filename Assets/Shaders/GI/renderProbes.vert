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
out vec3 WorldPos;
out vec4 Normal;

uniform mat4 V;
uniform mat4 M;
uniform mat4 P;


void main()
{	
    vec4 viewPos = V * M * vec4(vertexPosition_modelspace,1);
    WorldPos = vertexPosition_modelspace;
    FragPos = viewPos.xyz; 
    UV = vertexUV;
    Normal = V * vec4(vertexNormal_modelspace,0);
    gl_Position = P * viewPos;
}