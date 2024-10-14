#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;  // Stores both albedo and specular in one vector

#define MAXLIGHTS 26


in vec2 UV;
in mat3 TBN; // Tangent-Bitangent-Normal matrix\
in vec3 worldPos;

in vec4 FragPos;


uniform sampler2D DiffuseTextureSampler;
uniform sampler2D NormalTextureSampler;
uniform sampler2D RoughnessTextureSampler;
uniform sampler2D MetalicTextureSampler;

uniform float Roughness;
uniform float Metalic;

uniform vec3 LightColors[MAXLIGHTS];
uniform vec3 LightPositions_worldspace[MAXLIGHTS];
uniform vec3 Lightdirection[MAXLIGHTS];
uniform float LightLinears[MAXLIGHTS];
uniform float LightQuadratics[MAXLIGHTS];
uniform float LightRadius[MAXLIGHTS];
uniform float LightCutOff[MAXLIGHTS];
uniform float LightOuterCutOff[MAXLIGHTS];
uniform samplerCube depthMap[MAXLIGHTS];


uniform vec3 viewPos;

const float PI = 3.14159265359;
const float far_plane = 25.0; // Constant, moved outside main
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos, int index)
{
    vec3 fragToLight = fragPos - LightPositions_worldspace[index];
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap[index], fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);  
    
    return shadow;
}

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness) {
    float k = (roughness + 1.0);
    k = (k * k) / 8.0;

    return NdotV / (NdotV * (1.0 - k) + k);
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------



void main() {


    vec3 albedo = texture(DiffuseTextureSampler, UV).rgb;
    float alpha = texture(DiffuseTextureSampler, UV).a;

    float roughness = Roughness;
    if(roughness == -1)
        roughness = texture(RoughnessTextureSampler, UV).r;

    float metallic = Metalic;
    if(metallic == -1)
        metallic =texture(MetalicTextureSampler, UV).r;
    vec3 normalMap = texture(NormalTextureSampler, UV).rgb;
    normalMap = normalMap * 2.0 - 1.0; // Convert from [0,1] range to [-1,1]

    vec3 transformedNormal = normalize(TBN * normalMap);
    vec3 N = normalize(TBN * normalMap);
    vec3 V = normalize(viewPos - FragPos.xyz);

    // Reflectance at normal incidence
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < MAXLIGHTS; ++i) {
        if (LightRadius[i] == 0) continue;

        // Calculate distance between light and fragment
        vec3 L = normalize(LightPositions_worldspace[i] - FragPos.xyz);
        float distance = length(LightPositions_worldspace[i] - FragPos.xyz);
        float attenuation = 1.0 / (1.0 + LightLinears[i] * distance + LightQuadratics[i] * (distance * distance));
        vec3 radiance = LightColors[i] * attenuation;

        // Cook-Torrance BRDF
        vec3 H = normalize(V + L);
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = (1.0 - kS) * (1.0 - metallic);
        float NdotL = max(dot(N, L), 0.0);
        float shadow = ShadowCalculation(FragPos.xyz , i);
        //the 1.3 makes it a little brighter
        Lo += ((kD * albedo) * (1.0f - shadow) / PI + specular) * radiance * NdotL * (1.0f - shadow);

    }

    vec3 ambient = (vec3(10) * albedo);
    vec3 color = ambient * Lo;

    // HDR
    color = color / (color + vec3(1.0));
    gAlbedoSpec = vec4(color, alpha);
}