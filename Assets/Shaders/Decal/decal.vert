#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;

out vec3 DecalCenterPosition;

uniform mat4 V; // View matrix
uniform mat4 M; // Model matrix
uniform mat4 P; // Projection matrix

void main()
{
    // Compute the transformed vertex position in one go
    gl_Position = P * V * M * vec4(vertexPosition_modelspace, 1.0);

    // Extract the translation part of the model matrix (the decal center position)
    DecalCenterPosition = vec3(M[3]);
}