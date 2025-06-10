#version 430 core

layout (location = 0) out vec4 gTransparent;  // Stores both albedo and specular in one vector
layout (location = 1) out vec4 gData;  // Stores both albedo and specular in one vector

#define MAXLIGHTS 26

//transparent

in vec2 UV;
in mat3 TBN; // Tangent-Bitangent-Normal matrix\
in vec3 worldPos;
in vec4 viewFragPos;

in vec3 Normal;
in vec3 FragN;

in vec4 FragPos;


layout(binding = 0) uniform sampler2D DiffuseTextureSampler;
layout(binding = 1) uniform sampler2D NormalTextureSampler;
layout(binding = 2) uniform sampler2D RoughnessTextureSampler;
layout(binding = 3) uniform sampler2D MetalicTextureSampler;
layout(binding = 4) uniform samplerCube envMap;

layout(binding = 5 )uniform sampler2D uDepthMap;


uniform float Roughness;
uniform float Metalic;


struct Light{
    vec3 position;
    vec3 color;
    float strength;
    float radius;
    samplerCube depthMap;
};
uniform Light[MAXLIGHTS] lights;


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



float ShadowCalculation(vec3 fragPos, int index, vec3 N){
     vec3 fragToLight = fragPos - lights[index].position;
     vec3 lightDir = lights[index].position - fragPos;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    //float bias = 0.2;
    //float bias =0.1  ;
    float bias = max(0.1 * (1.0 - dot(N, normalize(lightDir))), 0.005f);

    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / lights[index].radius)) / 200;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(lights[index].depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= lights[index].radius;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);  
    
    return 1.0f - shadow;

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
vec3 Tonemap_ACES(const vec3 x) { // Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return (x * (a * x + b)) / (x * (c * x + d) + e);
}

// Function to linearize depth
float LinearizeDepth(float depth, float near, float far) {
    float z = depth * 2.0 - 1.0; // Back to NDC [-1, 1]
    return (2.0 * near * far) / (far + near - z * (far - near));
}


void main() {
    

    vec2 uv = gl_FragCoord.xy / vec2(textureSize(uDepthMap, 0));

    float fragDepth = -viewFragPos.z;
    vec3 backgroundPos = texture(uDepthMap, uv).rgb;
    float sampleDepth = -backgroundPos.z;



    if(fragDepth > sampleDepth && sampleDepth != 0)
        discard;


    vec3 albedo =  pow(texture(DiffuseTextureSampler, UV).rgb,vec3(2.2));
    float alpha = texture(DiffuseTextureSampler, UV).a;

    float roughness = Roughness;
    //if(roughness == -1)
        //roughness = texture(RoughnessTextureSampler, UV).r;
    float metallic = Metalic;
    //if(metallic == -1)
       // metallic =texture(MetalicTextureSampler, UV).r;
    vec3 normalMap = vec3(128, 128, 255)/vec3(255); //texture(NormalTextureSampler, UV).rgb;
    normalMap = normalMap * 2.0 - 1.0; // Convert from [0,1] range to [-1,1]

    vec3 transformedNormal = normalize(TBN * normalMap);
    vec3 V = normalize(viewPos - FragPos.xyz);

    // Reflectance at normal incidence
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < MAXLIGHTS; ++i) {
        if (lights[i].radius == 0) continue;

        // Calculate light direction and distance
        vec3 lightVec = lights[i].position - FragPos.xyz;
        float distance = length(lightVec);
        vec3 L = normalize(lightVec);
    
        // Correct attenuation using inverse square law with smooth fadeout
        float attenuation = 1.0 / (distance * distance + 0.001);
        float fade = 1.0 - smoothstep(lights[i].radius * 0.8, lights[i].radius, distance);
        vec3 radiance = lights[i].strength * lights[i].color * attenuation * fade;

        // Cook-Torrance BRDF
        vec3 H = normalize(V + L);
        float NDF = DistributionGGX(FragN, H, roughness);
        float G = GeometrySmith(FragN, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(FragN, V), 0.0) * max(dot(FragN, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kD = (1.0 - F) * (1.0 - metallic);
        float NdotL = max(dot(FragN, L), 0.0);

        // Correct light contribution (removed redundant shadow multiplication)
        float shadow = ShadowCalculation(FragPos.xyz, i, FragN);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL * shadow;
    }
    
    
    vec3 color = Lo;
    float intensity = dot(color, vec3(0.2126, 0.7152, 0.0722)); 
    float Changedalpha = 0.1 * intensity ; 

    // Tone mapping and gamma correction FIRST
   
    color = mix(color, Tonemap_ACES(color), 1.0);   
    //color = color / (color  + vec3(1.0));

    color = pow(color, vec3(1.0/2.2));


    // Calculate refraction direction (simplified)
    vec3 viewDir = normalize(-FragPos.xyz);
    vec3 refractDir = refract(viewDir, FragN, 1.0 / 1.5);

    // Apply distortion to UVs
    vec2 distortedUV =  (refractDir.xy * 0.05);

    // Sample the background scene with distortion

    // Optional: Add fresnel effect for more realism
    //float edgeScale = 1.0 - smoothstep(0.4, 0.45, length(UV - 0.5));
    //distortedUV *= mix(1.0, 1.0 + 2.0 * 0.2, 0.3);
    


    gTransparent = vec4(color * 2 + 0.01,0.2);
    gData = vec4(distortedUV,0,0.2);

}