#version 430 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;  


layout (binding = 0) uniform sampler2D diffuse;
layout (binding = 1) uniform sampler2D normal;
layout (binding = 2) uniform sampler2D roughness;
layout (binding = 3) uniform sampler2D metalic;

layout(rgba16f, binding = 6)  uniform image3D probeGrid;


uniform float Roughness;
uniform float Metalic;
uniform vec3 color;

uniform vec3 position;
uniform vec3 gridWorldPos;
uniform vec3 volume;
uniform vec3 spacing;
uniform int probeID;

in vec2 UV;
in vec3 FragPos;
in vec3 Normal;
in vec3 TrueNormal;
uniform bool HasNormalMap = true;


float LinearizeDepth(float depth) {
    float zNear = 0.001;          // Near plane
    float zFar = 15.0;         // Far plane
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

void main()
{    
    vec3 MaterialDiffuseColor = texture(diffuse, UV).rgb;

    float MaterialRoughness = Roughness;
    if(MaterialRoughness == -1)
        MaterialRoughness = texture(roughness, UV).r;

    float MaterialMetalic = Metalic;
    if(MaterialRoughness == -1)
        MaterialMetalic =texture(metalic, UV).r;
    
    // Sample the normal map and transform it to world space using the TBN matrix
    vec3 normalMap = texture(normal, UV).rgb;
    normalMap = normalMap * 2.0 - 1.0; // Convert from [0,1] range to [-1,1]

     float metallic  = MaterialMetalic;
    float roughness = MaterialRoughness;

    vec3 N = normalize(Normal);

    gPosition = vec4(FragPos,1);
     if(HasNormalMap)
        gNormal = N;
    else
       gNormal = TrueNormal;
    if(color != vec3(0)){
        gAlbedo = color;
    }
    else{
        gAlbedo = MaterialDiffuseColor;
    }
    gl_FragDepth =  LinearizeDepth(gl_FragCoord.z);

    vec3 pos = (position - gridWorldPos) / spacing;
	ivec3 texturePosition = ivec3(floor(pos));
	imageStore(probeGrid, texturePosition, vec4(probeID));


}