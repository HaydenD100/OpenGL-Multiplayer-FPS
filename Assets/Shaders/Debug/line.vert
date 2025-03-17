#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 V;
uniform mat4 P;

void main() {
    gl_Position = P * V * vec4(aPos, 1.0);
}