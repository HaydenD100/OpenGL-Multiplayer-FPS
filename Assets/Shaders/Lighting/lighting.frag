#version 430 core
layout (location = 0) out vec4 gLighting;

layout(std430, binding = 7) buffer ShCoeffient {
   mat3 L1SH_0[3750];
   mat3 L1SH_1[3750];
   mat3 L1SH_2[3750];
   float Depth[3750 * 4 * 4];


};

layout(binding = 6) uniform sampler3D probeGrid;



uniform vec3 gridWorldPos;
uniform vec3 volume;
uniform float spacing;


in vec2 UV;
//becuase we are binding cubemaps per light and the max texture bindings is 32 we have to limit the lights to 26
#define MAXLIGHTS 26

layout(binding = 0) uniform sampler2D gPostion;    // View-space position
layout(binding = 1) uniform sampler2D gNormal;     // View-space normal
layout(binding = 2) uniform sampler2D gAlbeido;
layout(binding = 3) uniform sampler2D gPBR;
layout(binding = 4) uniform sampler2D ssaoTexture;
layout(binding = 5) uniform sampler2D gTrueNormal;


uniform sampler2D gFinal; 


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

uniform bool isDead;

const float PI = 3.1415926535897932384626433832795;
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
    float zNear = 0.1;          // Near plane
    float zFar = 100.0;         // Far plane
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

#define myT vec3
#define myL 2
#define SphericalHarmonicsTL(T, L) T[(L + 1)*(L + 1)]
#define SphericalHarmonics SphericalHarmonicsTL(myT, myL)
#define shSize(L) ((L + 1)*(L + 1))
#define reflectTex iChannel0

// Constants
const float PIH = 1.5707963267948966192313216916398;
const float sqrtPI = 1.7724538509055160272981674833411; //sqrt(PI)
const float goldenAngle = 2.3999632297286533222315555066336; // PI * (3.0 - sqrt(5.0));
const vec3 LUMA = vec3(0.2126, 0.7152, 0.0722);



SphericalHarmonics shZero() {
	SphericalHarmonics result;
	for (int i = 0; i < shSize(myL); ++i)
	{
		result[i] = myT(0.0);
	}
    return result;
}


void shScale(inout SphericalHarmonics sh, myT scale) {
	for (int i = 0; i < shSize(myL); ++i)
	{
		sh[i] *= scale;
	}
}

vec3 SpherePoints_GoldenAngle(float i, float numSamples) {
    float theta = i * goldenAngle;
    float z = (1.0 - 1.0 / numSamples) * (1.0 - 2.0 * i / (numSamples - 1.0));
    float radius = sqrt(1.0 - z * z);
    return vec3(radius * vec2(cos(theta), sin(theta)), z);
}

void shAddWeighted(inout SphericalHarmonics accumulatorSh, in SphericalHarmonics sh, myT weight)
{
	for (int i = 0; i < shSize(myL); ++i)
	{
		accumulatorSh[i] += sh[i] * weight;
	}
}

SphericalHarmonics shEvaluate(vec3 p)
{
	// From Peter-Pike Sloan's Stupid SH Tricks
	// http://www.ppsloan.org/publications/StupidSH36.pdf
	// https://github.com/dariomanesku/cmft/blob/master/src/cmft/cubemapfilter.cpp#L130

	SphericalHarmonics result;

	float x = -p.x;
	float y = -p.y;
	float z = p.z;

	float x2 = x*x;
	float y2 = y*y;
	float z2 = z*z;

	float z3 = z2*z;

	float x4 = x2*x2;
	float y4 = y2*y2;
	float z4 = z2*z2;

	int i = 0;

	result[i++] =  myT( 1.0f/(2.0f*sqrtPI) );

	#if (myL >= 1)
		result[i++] = myT(-sqrt(3.0f/(4.0f*PI))*y );
		result[i++] = myT( sqrt(3.0f/(4.0f*PI))*z );
		result[i++] = myT(-sqrt(3.0f/(4.0f*PI))*x );        	
    #endif

	#if (myL >= 2)
		result[i++] = myT( sqrt(15.0f/(4.0f*PI))*y*x );
		result[i++] = myT(-sqrt(15.0f/(4.0f*PI))*y*z );
		result[i++] = myT( sqrt(5.0f/(16.0f*PI))*(3.0f*z2-1.0f) );
		result[i++] = myT(-sqrt(15.0f/(4.0f*PI))*x*z );
		result[i++] = myT( sqrt(15.0f/(16.0f*PI))*(x2-y2) );			
    #endif

	#if (myL >= 3)
		result[i++] = myT(-sqrt( 70.0f/(64.0f*PI))*y*(3.0f*x2-y2) );
		result[i++] = myT( sqrt(105.0f/ (4.0f*PI))*y*x*z );
		result[i++] = myT(-sqrt( 21.0f/(16.0f*PI))*y*(-1.0f+5.0f*z2) );
		result[i++] = myT( sqrt(  7.0f/(16.0f*PI))*(5.0f*z3-3.0f*z) );
		result[i++] = myT(-sqrt( 42.0f/(64.0f*PI))*x*(-1.0f+5.0f*z2) );
		result[i++] = myT( sqrt(105.0f/(16.0f*PI))*(x2-y2)*z );
		result[i++] = myT(-sqrt( 70.0f/(64.0f*PI))*x*(x2-3.0f*y2) );			
    #endif

	#if (myL >= 4)
		result[i++] = myT( 3.0f*sqrt(35.0f/(16.0f*PI))*x*y*(x2-y2) );
		result[i++] = myT(-3.0f*sqrt(70.0f/(64.0f*PI))*y*z*(3.0f*x2-y2) );
		result[i++] = myT( 3.0f*sqrt( 5.0f/(16.0f*PI))*y*x*(-1.0f+7.0f*z2) );
		result[i++] = myT(-3.0f*sqrt(10.0f/(64.0f*PI))*y*z*(-3.0f+7.0f*z2) );
		result[i++] = myT( (105.0f*z4-90.0f*z2+9.0f)/(16.0f*sqrtPI) );
		result[i++] = myT(-3.0f*sqrt(10.0f/(64.0f*PI))*x*z*(-3.0f+7.0f*z2) );
		result[i++] = myT( 3.0f*sqrt( 5.0f/(64.0f*PI))*(x2-y2)*(-1.0f+7.0f*z2) );
		result[i++] = myT(-3.0f*sqrt(70.0f/(64.0f*PI))*x*z*(x2-3.0f*y2) );
		result[i++] = myT( 3.0f*sqrt(35.0f/(4.0f*(64.0f*PI)))*(x4-6.0f*y2*x2+y4) );
    #endif

	return result;
}




#define ENV_SMPL_NUM 256



#define NORM2SNORM(value) (value * 2.0 - 1.0)
#define SNORM2NORM(value) (value * 0.5 + 0.5)

vec3 EquirectToDirection(vec2 uv) {
    uv = NORM2SNORM(uv);
    uv.x *= PI;  // phi
    uv.y *= PIH; // theta
        
    /* Calculate a direction from spherical coords:
	** R = 1
    ** x = R sin(phi) cos(theta)
	** y = R sin(phi) sin(theta)
	** z = R cos(phi)
	*/
    return vec3(cos(uv.x)*cos(uv.y)
              , sin(uv.y)
              , sin(uv.x)*cos(uv.y));
}

myT shDot(in SphericalHarmonics shA, in SphericalHarmonics shB)
{
	myT result = myT(0.0);
	for (int i = 0; i < shSize(myL); ++i)
	{
		result += shA[i] * shB[i];
	}
	return result;
}

myT shEvaluateDiffuse(SphericalHarmonics sh, vec3 direction) {

	SphericalHarmonics directionSh = shEvaluate(direction);
	// https://cseweb.ucsd.edu/~ravir/papers/envmap/envmap.pdf equation 8

	const float A[5] = float[5](
		 1.0,
		 2.0 / 3.0,
		 1.0 / 4.0,
		 0.0,
		-1.0f / 24.0
	);

	int i = 0;

	myT result = sh[i] * directionSh[i] * A[0]; ++i;

	#if (myL >= 1)
		result += sh[i] * directionSh[i] * A[1]; ++i;
		result += sh[i] * directionSh[i] * A[1]; ++i;
		result += sh[i] * directionSh[i] * A[1]; ++i;
	#endif

	#if (myL >= 2)
		result += sh[i] * directionSh[i] * A[2]; ++i;
		result += sh[i] * directionSh[i] * A[2]; ++i;
		result += sh[i] * directionSh[i] * A[2]; ++i;
		result += sh[i] * directionSh[i] * A[2]; ++i;
		result += sh[i] * directionSh[i] * A[2]; ++i;
	#endif

	// L3 and other odd bands > 1 have 0 factor

	#if (myL >= 4)
		i = 16;

		result += sh[i] * directionSh[i] * A[4]; ++i;
		result += sh[i] * directionSh[i] * A[4]; ++i;
		result += sh[i] * directionSh[i] * A[4]; ++i;
		result += sh[i] * directionSh[i] * A[4]; ++i;
		result += sh[i] * directionSh[i] * A[4]; ++i;
		result += sh[i] * directionSh[i] * A[4]; ++i;
		result += sh[i] * directionSh[i] * A[4]; ++i;
		result += sh[i] * directionSh[i] * A[4]; ++i;
		result += sh[i] * directionSh[i] * A[4]; ++i;
	#endif

	return result;   
}

vec3 GetRadianceFromSH(SphericalHarmonics shRadiance, vec3 direction) {
    SphericalHarmonics shDirection = shEvaluate(direction);

    vec3 sampleSh = max(vec3(0.0), shDot(shRadiance, shDirection));    
    return sampleSh;
}

vec3 GetIrradianceFromSH(SphericalHarmonics shRadiance, vec3 direction) {
    SphericalHarmonics shDirection = shEvaluate(direction);

    vec3 sampleIrradianceSh = max(vec3(0.0), shEvaluateDiffuse(shRadiance, direction));
    return sampleIrradianceSh;

}

vec3 GetProbe(vec3 fragWorldPos, ivec3 offset, out float weight, vec3 Normal) {
    vec3 gridCoords = (fragWorldPos - gridWorldPos) / spacing;
    ivec3 base = ivec3(floor(gridCoords));
    vec3 a = gridCoords - base;
    //int id = int(imageLoad(probeGrid, base + offset).x);
    int probe_id = int(texelFetch(probeGrid, base + offset,0).r);
    vec3 probe_worldPos = (base + offset) + gridWorldPos * spacing;


    SphericalHarmonics shRadiance;
    shRadiance[0] = L1SH_0[probe_id][0];
    shRadiance[1] = L1SH_0[probe_id][1];
    shRadiance[2] = L1SH_0[probe_id][2];

    shRadiance[3] = L1SH_1[probe_id][0];
    shRadiance[4] = L1SH_1[probe_id][1];
    shRadiance[5] = L1SH_1[probe_id][2];

    shRadiance[6] = L1SH_2[probe_id][0];
    shRadiance[7] = L1SH_2[probe_id][1];
    shRadiance[8] = L1SH_2[probe_id][2];

    //vec3 v = (probe_worldPos - fragWorldPos);
    vec3 dir = fragWorldPos - probe_worldPos;
    vec3 probe_color =  GetRadianceFromSH(shRadiance, dir);

    

    vec3 v = normalize(probe_worldPos - fragWorldPos); // TODO: no need to normalize if only checking sign
    float vdotn = dot(v, Normal);
    vec3 weights = mix(1. - a, a, offset);
    if(vdotn > 0)
        weight = weights.x * weights.y * weights.z;
    else
        weight = 0.;        

    //return probe_worldPos;
    //return gridCoords;  
    return probe_color;
}

vec3 GetIndirectLighting(vec3 WorldPos, vec3 Normal) { // Interpolate visible probes
    float w;
    vec3 light;
    float sumW = 0.;
    vec3 indirectLighting = vec3(0.);
    light = GetProbe(WorldPos, ivec3(0, 0, 0), w, Normal);
    indirectLighting += w * light;
    sumW += w;
    light = GetProbe(WorldPos, ivec3(0, 0, 1), w, Normal);
    indirectLighting += w * light;
    sumW += w;
    light = GetProbe(WorldPos, ivec3(0, 1, 0), w, Normal);
    indirectLighting += w * light;
    sumW += w;
    light = GetProbe(WorldPos, ivec3(0, 1, 1), w, Normal);
    indirectLighting += w * light;
    sumW += w;
    light = GetProbe(WorldPos, ivec3(1, 0, 0), w, Normal);
    indirectLighting += w * light;
    sumW += w;
    light = GetProbe(WorldPos, ivec3(1, 0, 1), w, Normal);
    indirectLighting += w * light;
    sumW += w;
    light = GetProbe(WorldPos, ivec3(1, 1, 0), w, Normal);
    indirectLighting += w * light;
    sumW += w;
    light = GetProbe(WorldPos, ivec3(1, 1, 1), w, Normal);
    indirectLighting += w * light;
    sumW += w;
    indirectLighting /= sumW;
    
    
    return indirectLighting;
}



void main() {
    float threshold = 0.0009; // Distance threshold
    if (length(UV - vec2(0.5, 0.5)) <= threshold) {
        gLighting = vec4(1);
        return;
    }



    // Retrieve data from G-buffer
    vec3 FragPos_view = texture(gPostion, UV).rgb;
    vec3 Normal_view = normalize(texture(gNormal, UV).rgb);


    vec3 albedo = texture(gAlbeido, UV).rgb;

    float skybox = texture(gPBR, UV).z;

    if(skybox == 1){
        if(isDead)
            albedo = albedo  + vec3(1,-0.2,-0.2);
        gLighting = vec4(albedo, 1);
        return; // Stop further execution and write this color to the framebuffer
    }

    float metallic  = texture(gPBR, UV).y;
    float roughness = texture(gPBR, UV).x;
    float ao = texture(ssaoTexture, UV).r;

    // Transform view-space position to world-space position
    vec3 FragPos = (inverseV * vec4(FragPos_view, 1.0)).xyz;
    vec3 N = normalize((transpose(V) * vec4(Normal_view, 0.0)).xyz);
    vec3 Vpos = normalize(viewPos - FragPos);

    // Reflectance at normal incidence
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 Lo = vec3(0.0);
    vec3 spec = vec3(0.0);
    for (int i = 0; i < MAXLIGHTS; ++i) {
        if (LightRadius[i] == 0) continue;

        // Calculate distance between light and fragment
        vec3 L = normalize(LightPositions_worldspace[i] - FragPos);
        float distance = length(LightPositions_worldspace[i] - FragPos);
        float attenuation = 1.0 / (1.0 + LightLinears[i] * distance + LightQuadratics[i] * (distance * distance));
        vec3 radiance = LightColors[i] * attenuation;

        // Cook-Torrance BRDF
        vec3 H = normalize(Vpos + L);
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, Vpos, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, Vpos), 0.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, Vpos), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = (1.0 - kS) * (1.0 - metallic);
        float NdotL = max(dot(N, L), 0.0);
        float shadow = ShadowCalculation(FragPos , i);
        //the 1.3 makes it a little brighter
        Lo += ((kD * albedo) * (1.0f - shadow) / PI + specular) * radiance * NdotL * (1.0f - shadow);
        spec += specular * radiance * NdotL * (1.0 - shadow);


    }
     
    vec3 trueNormal = texture(gTrueNormal, UV).rgb;

    vec3 indirectLighting = GetIndirectLighting(FragPos,trueNormal);

    vec3 adjustedIndirectLighting = indirectLighting;
    float factor = min(1, roughness * 1.5);
    adjustedIndirectLighting *= (0.6) * vec3(factor);
    adjustedIndirectLighting = max(adjustedIndirectLighting, vec3(0));
    adjustedIndirectLighting *= albedo.rgb * 1.5;
    
    vec3 ambient = vec3(ao);// + adjustedIndirectLighting ;
    vec3 color = ambient * (Lo + adjustedIndirectLighting);

    // HDR and gamma correction
    color = color / (color + vec3(1.0));
    // color = albedo.xyz;
    if(isDead)
        color = color + vec3(1,-0.2,-0.2);
        
    
    
    //not using this anymore
    //vec3 voxel_pos = clamp(FragPos,-100,100);
    //voxel_pos = (voxel_pos - (-100)) / (100 - (-100)) * 200;
    //ivec3 postionVoxel = ivec3(voxel_pos);
    //vec3 storedValue = vec3(imageLoad(voxelTextureColor, postionVoxel));
    //gLighting = vec4(vec3(ids[0]/ 4000.0f),1);
    //gLighting = vec4(indirectLighting ,1);
    //gLighting = vec4(adjustedIndirectLighting ,1);
    gLighting = vec4(color, spec);// + vec4(albedo * 0.2,1);
}
