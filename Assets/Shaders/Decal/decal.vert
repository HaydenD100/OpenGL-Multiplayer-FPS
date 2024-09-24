#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexTangent_modelspace;
layout(location = 4) in vec3 vertexBitangent_modelspace;

out vec4 posFS;
out vec4 posW;
out vec2 uvFS;

out vec3 DecalCenterPosition;
out vec2 TexCoords;

uniform mat4 V; // View matrix
uniform mat4 M; // Model matrix
uniform mat4 P; // Projection matrix
uniform mat3 normalMatrix3;



void main()
{	
    posW = M * vec4(vertexPosition_modelspace, 1);
    //Move position to clip space
    posFS = P * V * posW;
    uvFS = vertexUV;
    gl_Position = posFS;
    TexCoords = vertexUV;	
     
    DecalCenterPosition = vec3(M[3][0],M[3][1],M[3][2]);


}