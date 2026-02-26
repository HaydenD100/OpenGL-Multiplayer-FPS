#version 430 core
layout (location = 7) out vec4 gColor;
layout (location = 0) out vec4 gPosition;

layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;  // Stores both albedo and specular in one vector
layout (location = 3) out vec4 gRMA;  // Stores both albedo and specular in one vector
layout (location = 4) out vec4 gTrueNormal;  // Stores both albedo and specular in one vector
layout (location = 5) out vec4 gEmission;  // Stores both albedo and specular in one vector
layout (location = 6) out vec4 gTransparent;  // Stores both albedo and specular in one vector


in vec2 UV;
in vec3 FragPos;
in vec3 Normal;
in vec3 TrueNormal;
in mat3 TBN;
in vec2 T0;
in vec2 T1;
in float H0;


layout(binding = 0) uniform sampler2D DiffuseTextureSampler;
layout(binding = 1) uniform sampler2D NormalTextureSampler;
layout(binding = 2) uniform sampler2D RoughnessTextureSampler;
layout(binding = 3) uniform sampler2D MetalicTextureSampler;
layout(binding = 4) uniform sampler2D DefaultNormal;
layout(binding = 5) uniform sampler2D RandomNoise;
uniform bool IsEmissive = false;
uniform bool HasNormalMap = true;

uniform float Roughness;
uniform float Metalic;

uniform mat4 V;

uniform float Layer;
uniform float u_furLength = 0.001;
uniform float u_density = 10.0;

float LinearizeDepth(float z, float near, float far)
{
    float ndc = z * 2.0 - 1.0; // Convert to Normalized Device Coordinates [-1, 1]
    return (2.0 * near * far) / (far + near - ndc * (far - near));
}

float rand(float seed) {
    return fract(sin(seed) * 43758.5453123);
}
void main()
{    
    float alpha = 1;
    if(Layer != 0){
        const float MAX_LOD = 2.0;
        float wantLod = textureQueryLod(RandomNoise, UV).x;
        float lod = clamp(wantLod, 0.0, MAX_LOD);
        float randnoise = textureLod(RandomNoise, UV * u_density,lod).x;
        vec2 outUV = fract(UV *  u_density * 256.0) * 2.0 - 1.0;
        float mag = length(outUV);

        float layer01 = clamp(Layer / 96.0, 0.0, 1.0);
        float layer02 = clamp(Layer / 16.0, 0.0, 1.0);

        float magThreshold = 1.0 - smoothstep(0.2, 1.0, layer01);
        alpha = (1 - H0 * 1) * randnoise;
        alpha = clamp(alpha, 0, 1);

        alpha = 1- Layer / 96;
        if (randnoise < 0.9 || mag > magThreshold)
        {
            discard;
        }
    }
    

    // Sample the normal map and transform it to world space using the TBN matrix
    vec3 normalMap = texture(NormalTextureSampler, UV * 10).rgb;
    normalMap = normalMap * 2.0 - 1.0; // Convert from [0,1] range to [-1,1]
    vec3 transformedNormal = normalize(TBN * normalMap);
    
    if(isnan(transformedNormal.x) || isnan(transformedNormal.y) || isnan(transformedNormal.z)){
        transformedNormal = vec3(V * vec4(TrueNormal, 0));
    }
    // store the fragment position vector in the first gbuffer texture
    float nonLinearDepth = gl_FragCoord.z;
    float linearDepth = LinearizeDepth(nonLinearDepth, 0.0025, 200.0); // Use your camera near/far
    gPosition = vec4(FragPos.xyz,linearDepth);
    // also store the per-fragment normals into the gbuffer
    gRMA = vec4(1,0,0,0);
    if(HasNormalMap){
        gNormal = vec4(transformedNormal, 0);
    }
    else{
        gNormal = V * vec4(TrueNormal,0); //vec4(TrueNormal, 0);
    }
    
    
    vec3 furcolor = texture(DiffuseTextureSampler, UV).rgb;
    vec3 furcolor_offset = texture(DiffuseTextureSampler, T1).rgb;
    vec3 color = furcolor_offset - furcolor;
    //gColor = vec4(furcolor,1);
    
    gAlbedo = vec4(  furcolor, 1); 
    gTrueNormal = vec4(TrueNormal,0);  // Use transpose of TBN to inverse the transformation
    if(IsEmissive){
        gEmission = vec4(color, 1);
    }
    
}