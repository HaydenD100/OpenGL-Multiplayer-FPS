#version 330 core
out vec4 FragColor;

in vec2 UV;
#define MAXLIGHTS 60


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



uniform vec3 viewPos;
uniform mat4 inverseV; // Inverse of the view matrix
uniform mat4 V; // TT

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
void main()
{
    // Retrieve data from G-buffer
    vec3 FragPos_view = vec3(texture(gPostion, UV).rgb); // View-space position
    vec3 Normal_view = normalize(vec3(texture(gNormal, UV).rgb)); // View-space normal

    vec3 albedo = texture(gAlbeido, UV).rgb;
    float metallic  = texture(gPBR, UV).y;
    float roughness = texture(gPBR, UV).x;
    float ao = texture(ssaoTexture, UV).r;

    // Transform view-space position to world-space position
    vec4 FragPos_world = inverseV * vec4(FragPos_view, 1.0);
    FragPos_world /= FragPos_world.w; // Normalize by w
    vec3 FragPos = FragPos_world.xyz;

    vec3 N = normalize((transpose(V) * vec4(Normal_view, 0.0)).xyz);
    vec3 V = normalize(viewPos - FragPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);


    vec3 Lo = vec3(0);

    // Lighting calculations
    vec3 viewDir = normalize(viewPos - FragPos);

    for(int i = 0; i < MAXLIGHTS; ++i)
    {
        if(LightRadius[i] == 0)
            continue;

            // Calculate distance between light source and current fragment
        float distance = length(LightPositions_worldspace[i] - FragPos);
        
        // calculate per-light radiance
        vec3 L = normalize(LightPositions_worldspace[i] - FragPos);
        vec3 H = normalize(V + L);
        float attenuation = 1.0 / (1 + LightLinears[i] * distance + LightQuadratics[i] * (distance * distance)); 
        vec3 radiance = LightColors[i] * attenuation;

        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
            // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo +=  (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
        
    }

    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.8) * ao;
    
    vec3 color = ambient * Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma    
    FragColor = vec4(color, 1.0);
}