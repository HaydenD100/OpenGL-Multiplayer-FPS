#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;  // Stores both albedo and specular in one vector

in vec2 UV;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;


uniform sampler2D DiffuseTextureSampler;
uniform sampler2D NormalTextureSampler;
uniform sampler2D RoughnessTextureSampler;
uniform sampler2D MetalicTextureSampler;

uniform float Roughness;
uniform float Metalic;


void main()
{    
    vec3 MaterialDiffuseColor = texture(DiffuseTextureSampler, UV).rgb;

    float MaterialRoughness = Roughness;
    if(MaterialRoughness == -1)
        MaterialRoughness = texture(RoughnessTextureSampler, UV).r;

    float MaterialMetalic = Metalic;
    if(MaterialRoughness == -1)
        MaterialMetalic =texture(MetalicTextureSampler, UV).r;
    
    // Sample the normal map and transform it to world space using the TBN matrix
    vec3 normalMap = texture(NormalTextureSampler, UV).rgb;
    normalMap = normalMap * 2.0 - 1.0; // Convert from [0,1] range to [-1,1]
    vec3 transformedNormal = normalize(TBN * normalMap);

    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = vec4(transformedNormal, MaterialMetalic);

    gAlbedoSpec = vec4(MaterialDiffuseColor, MaterialRoughness); // RGB for Albedo, R for Specular Intensity

}