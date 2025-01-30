#version 430 core
layout (location = 0) out vec3 color;


layout(binding = 0, r32f) uniform image3D voxelTextureColor; // 3D texture bound to image unit 0
//layout(binding = 1, r32f) uniform image3D voxelTextureNormal; 


//becuase we are binding cubemaps per light and the max texture bindings is 32 we have to limit the lights to 26
#define MAXLIGHTS 26

in Vertex
{
    vec3 texCoord;
    vec3 normal;
    vec3 FragPos;
} In;


uniform sampler2D DiffuseTextureSampler;
uniform sampler2D NormalTextureSampler;
uniform sampler2D RoughnessTextureSampler;
uniform sampler2D MetalicTextureSampler;

uniform float Roughness;
uniform float Metalic;
uniform uint volumeDimension;

const float PI = 3.14159265359;

uniform vec3 viewPos;
uniform mat4 inverseV; // Inverse of the view matrix
uniform mat4 V; // View matrix


uniform vec3 LightColors[MAXLIGHTS];
uniform vec3 LightPositions_worldspace[MAXLIGHTS];
uniform vec3 Lightdirection[MAXLIGHTS];
uniform float LightLinears[MAXLIGHTS];
uniform float LightQuadratics[MAXLIGHTS];
uniform float LightRadius[MAXLIGHTS];
uniform float LightCutOff[MAXLIGHTS];
uniform float LightOuterCutOff[MAXLIGHTS];

vec2 getScreenCoords(vec4 clipPosition, vec2 viewportSize) {
    // Transform from clip space to normalized device coordinates (NDC)
    vec3 ndc = clipPosition.xyz / clipPosition.w;

    // Transform NDC to screen coordinates
    vec2 screenCoords = (ndc.xy * 0.5 + 0.5) * viewportSize;

    return screenCoords;
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


void main()
{    
    vec3 MaterialDiffuseColor = texture(DiffuseTextureSampler, In.texCoord.xy).rgb;

    float MaterialRoughness = Roughness;
    if(MaterialRoughness == -1)
        MaterialRoughness = texture(RoughnessTextureSampler, In.texCoord.xy).r;

    float MaterialMetalic = Metalic;
    if(MaterialRoughness == -1)
        MaterialMetalic = texture(MetalicTextureSampler, In.texCoord.xy).r;
    

   
    float depth = gl_FragCoord.z * (200 - 1.0);
    vec2 voxelPosXY = gl_FragCoord.xy - 0.5;    
    
    ivec3 position = ivec3(voxelPosXY.x,voxelPosXY.y,depth);

    imageStore(voxelTextureColor, position, vec4(MaterialDiffuseColor, 1));
    //imageStore(voxelTextureNormal, position, vec4(In.normal, 1));

    //color = vec3(gl_FragCoord.xy - 0.5,1);
    color = vec3(MaterialDiffuseColor);
}