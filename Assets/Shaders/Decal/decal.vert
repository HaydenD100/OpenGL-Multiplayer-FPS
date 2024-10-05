#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in mat4 instanceModelMatrix;



out vec3 DecalCenterPosition;
out mat4 inverseM;

uniform mat4 V; // View matrix
uniform mat4 P; // Projection matrix


void main()
{
    gl_Position = P * V * instanceModelMatrix * vec4(vertexPosition_modelspace, 1.0);
    //maybe do this cpu side
    inverseM = inverse(instanceModelMatrix);

    // Compute the transformed vertex position in one go
   // gl_Position = P * V * M * vec4(vertexPosition_modelspace, 1.0);
    // Extract the translation part of the model matrix (the decal center position)
    DecalCenterPosition = vec3(instanceModelMatrix[3]);
}