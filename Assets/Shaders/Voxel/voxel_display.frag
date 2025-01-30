#version 430 core

uniform sampler3D voxelTexture;  // 3D texture containing voxel data
uniform sampler2D gPostion;
uniform mat4 inverseV;

in vec2 UV;  // The world-space position of the fragment (cube)
out vec4 FragColor;

uniform vec3 gridOrigin;  // Origin of the voxel grid
uniform float voxelSize;  // Size of each voxel
uniform vec3 gridResolution; // Resolution of the grid (voxels along each axis)



void main() {
    vec4 FragPos_view = vec4(texture(gPostion, UV).rgb,1);
    vec3 FragPos = (inverseV * FragPos_view).xyz;

    // Normalize the fragment position to texture coordinates
     //FragColor = vec4(1,0,0,1);


    
    vec3 gridPos = (vec3(FragPos) - vec3(0)) / voxelSize;
    gridPos = clamp(gridPos, vec3(0.0), vec3(gridResolution) - vec3(1.0));
    vec3 voxelCoord = vec3(floor(gridPos));

    FragColor = vec4(texture(voxelTexture, voxelCoord).rgb,1);
    //FragColor = vec4(voxelCoord,1);

}