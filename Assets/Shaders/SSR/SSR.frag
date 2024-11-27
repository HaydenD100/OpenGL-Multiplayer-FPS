#version 430 core
layout (location = 0) out vec4 gSSR;

in vec2 UV;

uniform sampler2D gFinal; 
uniform sampler2D gPostion; 
uniform sampler2D gNormal; 
uniform sampler2D gAlbedo; 
uniform sampler2D gRMA; 

uniform mat4 inverseV;
uniform mat4 P;
uniform mat4 inverseP;
uniform mat4 V;

const float step = 0.1;
const float minRayStep = 0.1;
const float maxSteps = 30;
const int numBinarySearchSteps = 10;
const float reflectionSpecularFalloffExponent = 1.0;

//Credits to imanolfotia for the code, you can find there video and the code at https://imanolfotia.com/blog/1



vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth)
{
    float depth;

    vec4 projectedCoord;
 
    for(int i = 0; i < numBinarySearchSteps; i++)
    {

        projectedCoord = P * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
        depth = textureLod(gPostion, projectedCoord.xy, 2).z;

 
        dDepth = hitCoord.z - depth;

        dir *= 0.5;
        if(dDepth > 0.0)
            hitCoord += dir;
        else
            hitCoord -= dir;    
    }

        projectedCoord = P * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
    return vec3(projectedCoord.xy, depth);
}

vec4 RayMarch(vec3 dir, inout vec3 hitCoord, out float dDepth)
{

    dir *= step;
 
 
    float depth;
    int steps;
    vec4 projectedCoord;

 
    for(int i = 0; i < maxSteps; i++)
    {
        hitCoord += dir;
 
        projectedCoord = P * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
        depth = textureLod(gPostion, projectedCoord.xy, 2).z;
        if(depth > 1000.0)
            continue;
 
        dDepth = hitCoord.z - depth;

        if((dir.z - dDepth) < 1.2)
        {
            if(dDepth <= 0.0)
            {   
                vec4 Result;
                Result = vec4(BinarySearch(dir, hitCoord, dDepth), 1.0);

                return Result;
            }
        }
        
        steps++;
    }
 
    
    return vec4(projectedCoord.xy, depth, 0.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}




void main()
{

	float Metallic = texture(gRMA, UV).g; // R: Roughness, G: Metallic, A: Ambient Occlusion

	// Skip non-metallic pixels
	if (Metallic < 0.01){
        discard;
    }
	    
	// Retrieve data from G-buffer
	vec2 texSize  = textureSize(gPostion, 0).xy;


	vec3 viewPos = texture(gPostion, UV).xyz;
	vec3 viewNormal = normalize(texture(gNormal, UV).xyz);
	vec3 albedo = texture(gFinal, UV).rgb;
    float spec = texture(gFinal, UV).a;

    vec3 F0 = vec3(0.04); 
    F0      = mix(F0, albedo, Metallic);
    vec3 Fresnel = fresnelSchlick(max(dot(normalize(viewNormal), normalize(viewPos)), 0.0), F0);

    vec3 reflected = normalize(reflect(normalize(viewPos), normalize(viewNormal)));

    vec3 hitPos = viewPos;
    float dDepth;
 
    vec3 wp = vec3(vec4(viewPos, 1.0) * inverseV);
    vec4 coords = RayMarch((reflected * max(minRayStep, -viewPos.z)), hitPos, dDepth);
 
 
    vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));
 
 
    float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);

    float ReflectionMultiplier = pow(Metallic, reflectionSpecularFalloffExponent) * 
                screenEdgefactor * 
                -reflected.z;
 
    // Get color
    vec3 SSR = textureLod(gFinal, coords.xy, 0).rgb * clamp(ReflectionMultiplier, 0.0, 0.9) * Fresnel;

	gSSR = vec4(SSR,Metallic);
}
