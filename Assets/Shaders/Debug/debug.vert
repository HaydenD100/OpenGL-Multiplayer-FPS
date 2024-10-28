#version 430 core
layout (location = 0) in vec3 aPos;
uniform mat4 P;
uniform mat4 V;


void main(){
	gl_Position = V * P * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}