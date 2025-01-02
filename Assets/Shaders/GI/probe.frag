#version 430 core
layout (location = 0) out vec3 cubeMap;

#define MAXLIGHTS 26


in vec2 UV;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;


layout (binding = 0) uniform sampler2D diffuse;
layout (binding = 1) uniform sampler2D normal;
layout (binding = 2) uniform sampler2D roughness;
layout (binding = 3) uniform sampler2D metalic;


uniform float Roughness;
uniform float Metalic;


uniform vec3 viewPos;

uniform vec3 LightColors[MAXLIGHTS];
uniform vec3 lightPos[MAXLIGHTS];
uniform vec3 Lightdirection[MAXLIGHTS];
uniform float LightLinears[MAXLIGHTS];
uniform float LightQuadratics[MAXLIGHTS];
uniform float LightRadius[MAXLIGHTS];
uniform float LightCutOff[MAXLIGHTS];
uniform float LightOuterCutOff[MAXLIGHTS];
uniform samplerCube depthMap[MAXLIGHTS];


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
    vec3 fragToLight = fragPos - lightPos[index];
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.2;

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

    vec3 N = normalize(TBN * normalMap);
    vec3 V = normalize(viewPos - FragPos);

    // Reflectance at normal incidence
    vec3 F0 = mix(vec3(0.04), MaterialDiffuseColor, metallic);

    vec3 Lo = vec3(0.0);
    vec3 spec = vec3(0.0);
    for (int i = 0; i < MAXLIGHTS; ++i) {
        if (LightRadius[i] == 0) continue;

        // Calculate distance between light and fragment
        vec3 L = normalize(lightPos[i] - FragPos);
        float distance = length(lightPos[i] - FragPos);
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
        float shadow = ShadowCalculation(FragPos , i);
        //the 1.3 makes it a little brighter
        Lo += ((kD * MaterialDiffuseColor) * (1.0f - shadow) / PI + specular) * radiance * NdotL * (1.0f - shadow);
        spec += specular * radiance * NdotL * (1.0 - shadow);


    }

    vec3 ambient = vec3(1) * MaterialDiffuseColor;
    vec3 color = ambient * Lo;

    // HDR and gamma correction
    color = color / (color + vec3(1.0));

    cubeMap = vec3(color);
    gl_FragDepth =  LinearizeDepth(gl_FragCoord.z);

}