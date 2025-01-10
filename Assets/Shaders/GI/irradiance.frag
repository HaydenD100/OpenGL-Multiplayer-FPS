#version 430 core
layout (location = 1) out vec4  irradianceOut;

in vec2 UV;
in vec3 FragPos;
in vec3 WorldPos;

layout (binding = 0) uniform samplerCube environmentMap;
layout (binding = 1) uniform samplerCube depthCubeMap;

uniform int probeID;
uniform vec3 probe_world_Pos;
uniform vec3 gridWorldPos;
uniform vec3 volume;
uniform float spacing;


//we could do this in a compute shader






layout(std430, binding = 7) buffer ShCoeffient {
    vec3 L1SH_0[3750];
    vec3 L1SH_1[3750];
    vec3 L1SH_2[3750];
    vec3 L1SH_3[3750];

    vec3 L1SH_4[3750];
    vec3 L1SH_5[3750];
    vec3 L1SH_6[3750];
    vec3 L1SH_7[3750];

    vec3 L1SH_8[3750 * 2];
    mat3 probeVisbilty[3750 * 2];
};

layout(rgba16f, binding = 6)  uniform image3D probeGrid;


//Credits to https://www.shadertoy.com/view/wtt3W2

#define myT vec3
#define myL 1
#define SphericalHarmonicsTL(T, L) T[(L + 1)*(L + 1)]
#define SphericalHarmonics SphericalHarmonicsTL(myT, myL)
#define shSize(L) ((L + 1)*(L + 1))
#define reflectTex iChannel0


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
    vec3 col = textureLod(environmentMap, p, b).rgb;
    
    // fake HDR
    //col *= 1.0 + 1.0 * smoothstep(0.5, 1.0, dot(LUMA, col));
    
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

    vec2 ts = vec2(textureSize(environmentMap, 0));
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