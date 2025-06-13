#version 430 core
layout (location = 1) out vec4  irradianceOut;

in vec2 UV;
in vec3 FragPos;
in vec3 WorldPos;

//layout (binding = 0) uniform samplerCube environmentMap;

layout (binding = 0) uniform samplerCube gDiffuse;
layout (binding = 1) uniform samplerCube gNormal;
layout (binding = 2) uniform samplerCube gPosition;
layout (binding = 3) uniform samplerCube depthCubeMap;

uniform int probeID;
uniform vec3 probe_world_Pos;
uniform vec3 gridWorldPos;
uniform vec3 volume;
uniform float spacing;


//NOT NEEDED  
layout(std430, binding = 7) buffer ShCoeffient {
    vec3 L1SH_0[10000];
    vec3 L1SH_1[10000];
    vec3 L1SH_2[10000];
    vec3 L1SH_3[10000];

    vec3 L1SH_4[10000];
    vec3 L1SH_5[10000];
    vec3 L1SH_6[10000];
    vec3 L1SH_7[10000];
    vec3 L1SH_8[10000];
};



layout(rgba16f, binding = 6)  uniform image3D probeGrid;


//Credits to https://www.shadertoy.com/view/wtt3W2

#define myT vec3
#define myL 3
#define SphericalHarmonicsTL(T, L) T[(L + 1)*(L + 1)]
#define SphericalHarmonics SphericalHarmonicsTL(myT, myL)
#define shSize(L) ((L + 1)*(L + 1))
#define reflectTex iChannel0

#define MAXLIGHTS 17

uniform vec3 LightColors[MAXLIGHTS];
uniform vec3 lightPos[MAXLIGHTS];
uniform vec3 Lightdirection[MAXLIGHTS];
uniform float LightLinears[MAXLIGHTS];
uniform float LightQuadratics[MAXLIGHTS];
uniform float LightRadius[MAXLIGHTS];
uniform float LightCutOff[MAXLIGHTS];
uniform float LightOuterCutOff[MAXLIGHTS];
uniform samplerCube depthMap[MAXLIGHTS];


// Constants
const float PI  = 3.1415926535897932384626433832795;
const float PIH = 1.5707963267948966192313216916398;
const float sqrtPI = 1.7724538509055160272981674833411; //sqrt(PI)

const float goldenAngle = 2.3999632297286533222315555066336; // PI * (3.0 - sqrt(5.0));
const vec3 LUMA = vec3(0.2126, 0.7152, 0.0722);

// Predefined 16 evenly spaced directions
const vec3 directions[16] = vec3[](
    normalize(vec3(1, 0, 0)), normalize(vec3(-1, 0, 0)),
    normalize(vec3(0, 1, 0)), normalize(vec3(0, -1, 0)),
    normalize(vec3(0, 0, 1)), normalize(vec3(0, 0, -1)),
    normalize(vec3(1, 1, 0)), normalize(vec3(-1, -1, 0)),
    normalize(vec3(1, 0, 1)), normalize(vec3(-1, 0, -1)),
    normalize(vec3(0, 1, 1)), normalize(vec3(0, -1, -1)),
    normalize(vec3(1, 1, 1)), normalize(vec3(-1, -1, -1)),
    normalize(vec3(-1, 1, 1)), normalize(vec3(1, -1, -1))
);



//------------------------------------DIRECT LIGHING--------------------

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
    float viewDistance = length(probe_world_Pos - fragPos);
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


vec3 DirectLighting(vec3 p, float b){
    vec3 diffuse = textureLod(gDiffuse, p, b).rgb;
    vec3 FragPos = textureLod(gPosition, p, b).rgb;

    vec3 N = normalize(textureLod(gNormal, p, b)).xyz;
    vec3 V = normalize(probe_world_Pos - FragPos);

    float metallic = 0.1;
    float roughness = 1;


    // Reflectance at normal incidence
    vec3 F0 = mix(vec3(0.04), diffuse, metallic);

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
        Lo += ((kD * diffuse) * (1.0f - shadow) / PI + specular) * radiance * NdotL * (1.0f - shadow);
        spec += specular * radiance * NdotL * (1.0 - shadow);


    }
    vec3 ambient = vec3(1) * diffuse;
    vec3 Lightcolor = ambient * Lo;

    // HDR and gamma correction
    //Lightcolor = Lightcolor / (Lightcolor + vec3(1.0));
    //Lightcolor = N;
    return Lightcolor;
    //return vec3(1);
}




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
vec3 sampleReflectionMap(vec3 p, float b) {
    //vec3 col = textureLod(gDiffuse, p, b).rgb;
    vec3 col = DirectLighting(p,b);
    return col;
}
vec3 sampleDepthMap(vec3 p, float b) {
    vec3 col = textureLod(depthCubeMap, p, b).rgb;
    return col;
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
SphericalHarmonics CubeMapToRadianceSH() {
    // Initialise sh to 0
    SphericalHarmonics shRadiance = shZero();

    vec2 ts = vec2(textureSize(gDiffuse, 0));
    float maxMipMap = log2(max(ts.x, ts.y));

    float lodBias = maxMipMap - 5.0;
    

    for (int i=0; i < ENV_SMPL_NUM; ++i) {
        vec3 direction = SpherePoints_GoldenAngle(float(i), float(ENV_SMPL_NUM));
        vec3 radiance = sampleReflectionMap(direction, lodBias);
        shAddWeighted(shRadiance, shEvaluate(direction), radiance);
    }

    // integrating over a sphere so each sample has a weight of 4*PI/samplecount (uniform solid angle, for each sample)
    float shFactor = 4.0 * PI / float(ENV_SMPL_NUM);
    shScale(shRadiance, vec3(shFactor));

    return shRadiance;
}

SphericalHarmonics DepthMapToRadianceSH() {
    // Initialise sh to 0
    SphericalHarmonics shRadiance = shZero();

    vec2 ts = vec2(textureSize(depthCubeMap, 0));
    float maxMipMap = log2(max(ts.x, ts.y));

    float lodBias = maxMipMap - 5.0;
    

    for (int i=0; i < ENV_SMPL_NUM; ++i) {
        vec3 direction = SpherePoints_GoldenAngle(float(i), float(ENV_SMPL_NUM));
        vec3 radiance = sampleDepthMap(direction, lodBias);
        shAddWeighted(shRadiance, shEvaluate(direction), radiance);
    }

    // integrating over a sphere so each sample has a weight of 4*PI/samplecount (uniform solid angle, for each sample)
    float shFactor = 4.0 * PI / float(ENV_SMPL_NUM);
    shScale(shRadiance, vec3(shFactor));

    return shRadiance;
}

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

vec2 encodeToOctahedralMap(vec3 dir) {
    // Normalize the direction vector
    dir = normalize(dir);

    // Encode to octahedral coordinates
    vec2 p = dir.xy / (abs(dir.x) + abs(dir.y) + abs(dir.z));
    if (dir.z < 0.0) {
        p = (1.0 - abs(p.yx)) * sign(p);
    }

    // Map [-1, 1] to [0, 3]
    p = (p * 0.5 + 0.5) * 3.0;

    return p;
}

float LinearizeDepth(float depth) {
    float zNear = 0.001;          // Near plane
    float zFar = 15.0;         // Far plane
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

vec3 clampToNearestDirection(vec3 position) {
    float maxDot = -1.0; // Initialize to the smallest possible value
    int bestMatchIndex = 0;

    // Iterate through all predefined directions
    for (int i = 0; i < 16; ++i) {
        float dotProduct = dot(position, directions[i]);

        // Find the direction with the largest dot product (smallest angle)
        if (dotProduct > maxDot) {
            maxDot = dotProduct;
            bestMatchIndex = i;
        }
    }

    // Return the closest direction
    return directions[bestMatchIndex];
}

int clampToNearestDirectionINT(vec3 position) {
    float maxDot = -1.0; // Initialize to the smallest possible value
    int bestMatchIndex = 0;

    // Iterate through all predefined directions
    for (int i = 0; i < 16; ++i) {
        float dotProduct = dot(position, directions[i]);

        // Find the direction with the largest dot product (smallest angle)
        if (dotProduct > maxDot) {
            maxDot = dotProduct;
            bestMatchIndex = i;
        }
    }

    // Return the closest direction
    return bestMatchIndex;
}

bool isAngle(vec3 position) {
    float maxDot = 1; // Initialize to the smallest possible value
    int bestMatchIndex = 0;

    // Iterate through all predefined directions
    for (int i = 0; i < 16; ++i) {
        float dotProduct = dot(position, directions[i]);

        // Find the direction with the largest dot product (smallest angle)
        if (dotProduct > maxDot) {
            return true;
        }
    }

    // Return the closest direction
    return false;
}


void main()
{		

     vec3 direction = WorldPos;
     // Normalized pixel coordinates (from 0 to 1)
     
	SphericalHarmonics shRadiance = CubeMapToRadianceSH();
	SphericalHarmonics shDepth = DepthMapToRadianceSH();


     vec3 col;
	 col =  GetRadianceFromSH(shRadiance, direction);
	
	L1SH_0[probeID] = shRadiance[0];
	L1SH_1[probeID] = shRadiance[1];
	L1SH_2[probeID] = shRadiance[2];
	L1SH_3[probeID] = shRadiance[3];

	#if (myL >= 2)
		L1SH_4[probeID] = shRadiance[4];
		L1SH_5[probeID] = shRadiance[5];
		L1SH_6[probeID] = shRadiance[6];
		L1SH_7[probeID] = shRadiance[7];
		L1SH_8[probeID] = shRadiance[8];
	#endif

	probeDepthEncoded[probeID] = mat4(shDepth[0].x,shDepth[1].x,shDepth[2].x,shDepth[3].x,
									  shDepth[4].x,shDepth[5].x,shDepth[6].x,shDepth[7].x,
									  shDepth[8].x,shDepth[9].x,shDepth[10].x,shDepth[11].x,
									  shDepth[12].x,shDepth[13].x,shDepth[14].x,shDepth[15].x);



	/*
	if(isAngle(direction)){

		int flattenedIndex = int(floor(probeID * 16 + clampToNearestDirectionINT(direction)));
		Depth[flattenedIndex] = texture(depthCubeMap,direction).x;
	}
	*/
	
	
		// I have to change this 
	for (int i = 0; i < 16; ++i) {
        int flattenedIndex = int(floor(probeID * 16 + i));
		//Depth[flattenedIndex] = texture(depthCubeMap,directions[i]).x;
    }
	

	vec3 pos = (probe_world_Pos - gridWorldPos) / spacing;

	ivec3 texturePosition = ivec3(floor(pos));
	imageStore(probeGrid, texturePosition, vec4(probeID));

}
