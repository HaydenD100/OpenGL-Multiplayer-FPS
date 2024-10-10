#version 430 core
out vec4 FragColor;

in vec2 UV;
//becuase we are binding cubemaps per light and the max texture bindings is 32 we have to limit the lights to 26
#define MAXLIGHTS 26

uniform sampler2D gPostion;    // View-space position
uniform sampler2D gNormal;     // View-space normal
uniform sampler2D gAlbeido;
uniform sampler2D gPBR;
uniform sampler2D ssaoTexture;

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
uniform mat4 inverseV; // Inverse of the view matrix
uniform mat4 V; // View matrix

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
    float threshold = 0.0009; // Distance threshold
    if (length(UV - vec2(0.5, 0.5)) <= threshold) {
        FragColor = vec4(1);
        return;
    }

    // Retrieve data from G-buffer
    vec3 FragPos_view = texture(gPostion, UV).rgb;
    vec3 Normal_view = normalize(texture(gNormal, UV).rgb);

    vec3 albedo = texture(gAlbeido, UV).rgb;
    float alpha = texture(gAlbeido, UV).a;

    float metallic  = texture(gPBR, UV).y;
    float roughness = texture(gPBR, UV).x;
    float ao = texture(ssaoTexture, UV).r;

    // Transform view-space position to world-space position
    vec3 FragPos = (inverseV * vec4(FragPos_view, 1.0)).xyz;
    vec3 N = normalize((transpose(V) * vec4(Normal_view, 0.0)).xyz);
    vec3 V = normalize(viewPos - FragPos);

    // Reflectance at normal incidence
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < MAXLIGHTS; ++i) {
        if (LightRadius[i] == 0) continue;

        // Calculate distance between light and fragment
        vec3 L = normalize(LightPositions_worldspace[i] - FragPos);
        float distance = length(LightPositions_worldspace[i] - FragPos);
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
        Lo += ((kD * albedo) * (1.0f - shadow) / PI + specular) * radiance * NdotL * (1.0f - shadow);

    }

    vec3 ambient = vec3(1.5) * ao;
    vec3 color = ambient * Lo;

    // HDR and gamma correction
    color = color / (color + vec3(1.0));
    FragColor = vec4(color, alpha);
}
