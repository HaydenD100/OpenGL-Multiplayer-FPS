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

uniform mat4 V;
uniform mat4 P;


void main()
{	
    WorldPos = vertexPosition_modelspace;
    UV = vertexUV;
    gl_Position = P * V * vec4(vertexPosition_modelspace,1);

}