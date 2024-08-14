#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexTangent_modelspace;
layout(location = 4) in vec3 vertexBitangent_modelspace;

out vec2 UV;
out vec3 Position_worldspace;
out mat3 tangentToWorld;


uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform mat3 MV3x3;


void main()
{	
    mat3 normalMatrix = transpose(inverse(mat3(M)));
    vec3 normal = normalMatrix * vertexNormal_modelspace;
    vec3 Tangent = normalize(gl_NormalMatrix[0]); 
    vec3 Binormal = normalize(gl_NormalMatrix[1]);

    tangentToWorld = mat3(Tangent.x, Binormal.x, normal.x,
                           Tangent.y, Binormal.y, normal.y,
                           Tangent.z, Binormal.z, normal.z);
     

	Position_worldspace = (M * vec4(vertexPosition_modelspace, 1)).xyz;
	UV = vertexUV;
	gl_Position =  MVP * vec4(vertexPosition_modelspace, 1);
}