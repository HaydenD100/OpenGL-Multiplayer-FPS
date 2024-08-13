#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexTangent_modelspace;
layout(location = 4) in vec3 vertexBitangent_modelspace;

out vec2 UV;
out vec3 Position_worldspace;
out vec3 normal;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform mat3 MV3x3;

void main()
{	
	Position_worldspace = (M * vec4(vertexPosition_modelspace, 1)).xyz;
	UV = vertexUV;
	mat3 normalMatrix = transpose(inverse(mat3(M)));
	normal = normalize(normalMatrix * vertexNormal_modelspace);

	gl_Position =  MVP * vec4(vertexPosition_modelspace, 1);
}