#version 330 core

in vec2 UV;

out vec3 color;

uniform sampler2D gPostion;
uniform sampler2D gNormal;
uniform sampler2D gAlbeido;
uniform sampler2D depthTexture;

void main(){
	color = texture(  gNormal, UV).xyz ;
}