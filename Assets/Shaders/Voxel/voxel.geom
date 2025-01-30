#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in Vertex
{
    vec3 texCoord;
    vec3 normal;
} In[3];

out GeometryOut
{
	vec3 wsPosition;
    vec3 position;
    vec3 normal;
    vec3 texCoord;
    flat vec4 triangleAABB;
} Out;

uniform mat4 VP;
uniform mat4 inverseVP;
uniform uint volumeDimension;

uniform float voxelScale;
uniform vec3 worldMinPoint;



void main()
{
	

	EndPrimitive();
}