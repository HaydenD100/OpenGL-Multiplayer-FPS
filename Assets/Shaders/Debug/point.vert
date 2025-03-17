#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 V;
uniform mat4 P;

void main()
{
    vec4 clipPos = P * V * vec4(aPos, 1.0);
    gl_Position = clipPos;
    float desiredScreenSize = 20.0; // Size in pixels (constant)
    gl_PointSize = desiredScreenSize / clipPos.w;
}