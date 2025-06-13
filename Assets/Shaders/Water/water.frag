#version 430 core
layout (location = 0) out vec4 gTransparent;  // Stores both albedo and specular in one vector
layout (location = 1) out vec4 gData;  // Stores both albedo and specular in one vector
layout (location = 2) out vec3 gPosition;  // Stores both albedo and specular in one vector

#define MAXLIGHTS 26

in vec2 UV;
in vec3 FragPos;
in vec3 N;
in vec4 viewFragPos;

layout(binding = 0) uniform samplerCube sky;    // View-space position
layout(binding = 1) uniform sampler2D uDepthMap;

uniform vec3 viewpos;
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform mat4 P;

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
float LinearizeDepth(float z, float near, float far)
{
    float ndc = z * 2.0 - 1.0; // Convert to Normalized Device Coordinates [-1, 1]
    return (2.0 * near * far) / (far + near - ndc * (far - near));
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

void main() {


    vec3 albedo = vec3(0, 1, 0.843);

    vec3 I = normalize(viewpos - FragPos);
    vec3 R = reflect(I, normalize(N));
    vec3 envColor = texture(sky, R).rgb * 0.2;
    float fresnel = pow(1.0 - dot(normalize(N), -I), 5.0);
    fresnel = clamp(fresnel, 0.0, 1.0);

    vec3 fresnelReflect = pow(envColor, vec3(2.2)); // Convert envColor to linear
    vec3 baseAlbedo = pow(albedo, vec3(2.2));       // Convert albedo to linear

    albedo = mix(baseAlbedo, fresnelReflect, fresnel);

    // Optional: convert back to sRGB
    // Tone mapping and gamma correction FIRST

    float roughness = 0.04;
    float metallic = 0.0;


    vec2 uv = gl_FragCoord.xy / vec2(textureSize(uDepthMap, 0));
    float fragDepth = -viewFragPos.z;
    vec3 backgroundPos = texture(uDepthMap, uv).rgb;
    float sampleDepth = -backgroundPos.z;



    if(fragDepth > sampleDepth && sampleDepth != 0)
        discard;

    vec3 V = normalize(viewPos - FragPos.xyz);
 // Transform view-space position to world-space position
    //vec3 N = trueNormal;
    vec3 Vpos = normalize(viewPos - FragPos);

    // Reflectance at normal incidence
    if(metallic == -1)
        metallic = 0;
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 Lo = vec3(0.0);
    vec3 spec = vec3(0.0);



    
    for (int i = 0; i < MAXLIGHTS; ++i) {
        if (lights[i].radius == 0) continue;

        // Calculate distance between light and fragment
        vec3 L = normalize(lights[i].position - FragPos);
        float distance = length(lights[i].position - FragPos);
        //float attenuation = 1.0 / (1.0 + lights[i].linear * distance + lights[i].quadratic * (distance * distance));
        //vec3 radiance = lights[i].color * attenuation;

        vec3 radiance = lights[i].strength * lights[i].color;// * 1.25;
	    float attenuation = smoothstep(lights[i].radius, 0,  distance);

        // Cook-Torrance BRDF
        vec3 H = normalize(Vpos + L);
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, Vpos, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, Vpos), 0.0), F0) * 1.5;

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, Vpos), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = (1.0 - kS) * (1.0 - metallic);
        float NdotL = max(dot(N, L), 0.0);
        float shadow = 1;
        //the 1.3 makes it a little brighter
        Lo += ((kD * albedo) * (shadow) / PI + specular) * radiance * NdotL * (shadow);
        spec += specular * radiance * NdotL * (shadow);
    }

            // Calculate distance between light and fragment
    vec3 L = normalize(vec3(50,50,0) - FragPos);
    float distance = length(vec3(50,50,0) - FragPos);
    //float attenuation = 1.0 / (1.0 + lights[i].linear * distance + lights[i].quadratic * (distance * distance));
    //vec3 radiance = lights[i].color * attenuation;

    //strength color
    vec3 radiance = 40.0f * vec3(1,1,1);// * 1.25;
    //radiuus = 200
	float attenuation = smoothstep(200, 0,  distance);

    // Cook-Torrance BRDF
    vec3 H = normalize(Vpos + L);
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, Vpos, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, Vpos), 0.0), F0) *  1.5;;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, Vpos), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = (1.0 - kS) * (1.0 - metallic);
    float NdotL = max(dot(N, L), 0.0);
    float shadow = 1;
    //the 1.3 makes it a little brighter
    Lo += ((kD * albedo) * (shadow) / PI + specular) * radiance * NdotL * (shadow);
    spec += specular * radiance * NdotL * (shadow);
    
    vec3 ambientColor = albedo * (vec3(0.1));
    vec3 ambientLighting = ambientColor * 10;

    // Ambient hack
	float amfactor = min(1, 1 - metallic * 1.0);
	ambientLighting *= (1.0) * vec3(amfactor);

    vec3 directlight = Lo + ambientLighting;
    vec3 color = directlight;

    color = mix(color, Tonemap_ACES(color), 1.0);   
    //color = color / (color  + vec3(1.0));

    color = pow(color, vec3(1.0/2.2));


    // Calculate refraction direction (simplified)
    vec3 viewDir = normalize(-FragPos.xyz);
    vec3 refractDir = refract(viewDir, N, 1.0 / 1.5);

    // Apply distortion to UVs
    vec2 distortedUV =  (refractDir.xy * 0.06);
    distortedUV = mix(distortedUV,distortedUV * 0.4,fresnel);
    // Sample the background scene with distortion

    // Optional: Add fresnel effect for more realism
    //float edgeScale = 1.0 - smoothstep(0.4, 0.45, length(UV - 0.5));
    //distortedUV *= mix(1.0, 1.0 + 2.0 * 0.2, 0.3);

    float nonLinearDepth = gl_FragCoord.z;
    float linearDepth = LinearizeDepth(nonLinearDepth, 0.0025, 200.0); // Use your camera near/far
    gTransparent = vec4(color + vec3(0,0.6,0.6) * 1 ,0.2);
    gData = vec4(distortedUV,linearDepth,0.9);
}