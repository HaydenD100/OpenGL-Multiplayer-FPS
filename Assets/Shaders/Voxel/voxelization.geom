#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices =(3)) out;

in vec3 FragPosGeom[];
in vec3 NormalGeom[];
in vec2 TexCoordsGeom[];
in float voxelizationAreaSizeGeom[];
in vec4 FragPosLightSpaceDirGeom[];
in vec3 camPosGeom[];

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out float voxelizationAreaSizeFrag;
out vec4 FragPosLightSpaceDir;
out vec3 camPosFrag;

void main(){

	// Here we are finding the dominant axis of a given triangle (the axis that the triangle is most aligned with) and projecting the triangle onto its best aligned plane.
	// we do that in order to maximize the amount of fragments generated per triangle so that a given triangle covers as many voxels as possible. Another thing
	// we can do to maybe make this better is scale the triangles up a little bit so that they fill voxels that would otherwise not be filled because the triangle
	// only intersects its edge.

	// conservative rasterization;
	float projectionScalingFactor = 1.0;

	const vec3 p1 = FragPosGeom[1] - FragPosGeom[0];
	const vec3 p2 = FragPosGeom[2] - FragPosGeom[0];
	const vec3 p = abs(cross(p1, p2)); 
	for(uint i = 0; i < 3; ++i){
		FragPos = gl_in[i].gl_Position.xyz;
		Normal = NormalGeom[i];
		TexCoords = TexCoordsGeom[i];
		voxelizationAreaSizeFrag = voxelizationAreaSizeGeom[i];
		camPosFrag = camPosGeom[i];
		FragPosLightSpaceDir = FragPosLightSpaceDirGeom[i];
		if(p.z > p.x && p.z > p.y){
			gl_Position = vec4(FragPos.x * projectionScalingFactor, FragPos.y * projectionScalingFactor, 0, 1);
		} else if (p.x > p.y && p.x > p.z){
			gl_Position = vec4(FragPos.y * projectionScalingFactor, FragPos.z * projectionScalingFactor, 0, 1);
		} else {
			gl_Position = vec4(FragPos.x * projectionScalingFactor, FragPos.z * projectionScalingFactor, 0, 1);
		}
		EmitVertex();
	}
    EndPrimitive();
}