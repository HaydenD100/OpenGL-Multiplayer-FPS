#version 430 core
layout (location = 0) in vec3 aPos;


uniform mat4 VP;
uniform vec3 color;

void main(){
	gl_Position = VP * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}