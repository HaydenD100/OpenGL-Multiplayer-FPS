#version 430 core
layout (location = 2) out vec4 gAlbedo;  // Stores both albedo and specular in one vector
uniform vec3 color;

void main(){
	 gAlbedo = vec4(color, 1);
}