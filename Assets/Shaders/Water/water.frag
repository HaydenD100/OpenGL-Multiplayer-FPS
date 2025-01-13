#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;  // Stores both albedo and specular in one vector
layout (location = 3) out vec4 gRMA;  // Stores both albedo and specular in one vector
layout (location = 4) out vec4 gTrueNormal;  // Stores both albedo and specular in one vector
layout (location = 5) out vec4 gEmission;  // Stores both albedo and specular in one vector


in vec2 UV;
in vec3 FragPos;
in vec3 Normal;
in vec3 TrueNormal;
in mat3 TBN;


layout (binding = 0) uniform sampler2D DiffuseTextureSampler;
layout (binding = 1) uniform sampler2D NormalTextureSampler;
layout (binding = 4) uniform sampler2D DefaultNormal;
layout (binding = 2) uniform sampler2D RoughnessTextureSampler;
layout (binding = 3) uniform sampler2D MetalicTextureSampler;

uniform float Roughness;
uniform float Metalic;

uniform mat4 V;

uniform bool IsEmissive;



void main()
{    
    vec3 MaterialDiffuseColor = texture(DiffuseTextureSampler, UV).rgb;

    float MaterialRoughness = Roughness;
    if(MaterialRoughness == -1)
        MaterialRoughness = texture(RoughnessTextureSampler, UV).r;

    float MaterialMetalic = Metalic;
    if(MaterialRoughness == -1)
        MaterialMetalic = texture(MetalicTextureSampler, UV).r;
    
    // Sample the normal map and transform it to world space using the TBN matrix
    vec3 normalMap = texture(NormalTextureSampler, UV).rgb;
    normalMap = normalMap * 2.0 - 1.0; // Convert from [0,1] range to [-1,1]
    vec3 transformedNormal = normalize(TBN * normalMap);
    

    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    gRMA = vec4(0.1,1,0,0);
    gNormal = vec4(transformedNormal, 0);
    gAlbedo = vec4(0,0,1, 0.2); // RGB for Albedo, R for Specular Intensity
    gTrueNormal = vec4(TrueNormal,0);  // Use transpose of TBN to inverse the transformation
    if(IsEmissive){
        gEmission = vec4(MaterialDiffuseColor, 1);
    }
        


}