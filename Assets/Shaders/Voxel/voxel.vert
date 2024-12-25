#version 430 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexTangent_modelspace;
layout(location = 4) in vec3 vertexBitangent_modelspace;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;


out Vertex
{
    vec3 texCoord;
    vec3 normal;
    vec3 FragPos;

};


uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform mat4 P;
uniform mat3 normalMatrix3;

const int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];
uniform bool animated;

uniform mat4 VP;
uniform mat4 inverseVP;
uniform uint volumeDimension;

uniform float voxelScale;
uniform vec3 worldMinPoint;


void main()
{	vec4 viewPos;
	vec4 worldPos;

    if(animated){
        mat4 BoneTransform = finalBonesMatrices[boneIds[0]] * weights[0];
        BoneTransform     += finalBonesMatrices[boneIds[1]] * weights[1];
        BoneTransform     += finalBonesMatrices[boneIds[2]] * weights[2];
        BoneTransform     += finalBonesMatrices[boneIds[3]] * weights[3];
        worldPos = M * BoneTransform * vec4(vertexPosition_modelspace,1);

        mat3 normalMatrix = transpose(inverse(mat3(V * M)));
        normal = normalize(normalMatrix * mat3(BoneTransform)  * vertexNormal_modelspace);
    }
    else{
        worldPos =  M * vec4(vertexPosition_modelspace,1.0f);
        mat3 normalMatrix = transpose(inverse(mat3(V * M)));
        normal = normalize(normalMatrix * vertexNormal_modelspace);
    }
    
    texCoord = vec3(vertexUV,0);
    //normalizedPosition = (vec3(worldPos) - vec3(-100)) / (vec3(100) - vec3(-100));


    //voxelPos = (normalizedPosition * (vec3(volumeDimension) - 1.0));


    // Transform to orthographic clip space for rasterization
    FragPos = vec3(worldPos);
    gl_Position = P * (worldPos);
    //gl_Position = P * (worldPos);

    gl_Position.xy *= 5;
    //gl_Position = P * vec4(voxelPos,1);

}