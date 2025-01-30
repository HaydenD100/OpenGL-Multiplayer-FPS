#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat3 normalMatrix;
uniform float voxelizationAreaSize;

// directional light shadows
uniform mat4 dirLightSpaceMatrix;
uniform vec3 camPos;
out vec4 FragPosLightSpaceDirGeom;

out float voxelizationAreaSizeGeom;
out vec3 FragPosGeom;
out vec3 NormalGeom;
out vec2 TexCoordsGeom;
out vec3 camPosGeom;

void main()
{
	FragPosGeom = vec3(model * vec4(aPos, 1.0));
	NormalGeom = normalMatrix * aNormal;
	TexCoordsGeom = aTexCoords;
	camPosGeom = camPos;
	voxelizationAreaSizeGeom = voxelizationAreaSize;
	FragPosLightSpaceDirGeom = dirLightSpaceMatrix * model * vec4(aPos, 1.0);

	// just for test, we are going to voxelize the scene 60 units in each direction. Here we are dividing the world position of the vertex by 1/30, which means
	// that components between -30 and 30 will me mapped to NDC [-1, 1]. In the geometry shader we are going to pass this position along to the fragment shader
	// in another variable (because we have to orthographically project gl_position on its dominant axis in order to maximize the number of fragments generated 
	// per triangle so that the triangles fill as many voxels as possible). Since the world coordinate below is going to be transferred to another variable before 
	// being passed along to the fragment shader, it is not going to be clipped automatically by opengl, this means that we will have to clip it manually in the 
	// fragment shader. After manually clipping the fragment's interpolated world position, we convert it to the [0, 1] range and divide the 3d texture's dimensions
	// by it. We will use that value to index voxels in the 3d texture and inject lighting information into them.

	gl_Position = vec4(FragPosGeom * 1.0 / (vec3(voxelizationAreaSize)), 1.0); 

}	