#version 460 core

#extension GL_NV_gpu_shader5 : require
#extension GL_NV_shader_atomic_fp16_vector : require

struct Material 
{
	sampler2D texture_albedo1;

	sampler2D texture_normal1;

	sampler2D texture_rma1;

	sampler2D texture_emission1;
};

uniform Material material;	
layout(rgba16f, binding = 0) uniform image3D texture3D;

struct Light
{
	vec4 position;
	vec4 color;
	vec4 direction;
	vec4 offset;
	float type;
	float radius;
	float innerCutoff;
	float outerCutoff;
	bool shadowCaster;
	bool isDirty;
};

layout(binding = 0, std430) buffer LightsSSBO 
{
	Light lights[];
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in float voxelizationAreaSizeFrag;

// misc
uniform bool hasAOTexture;
uniform vec2 textureScaling;
vec2 scaledTexCoords;
in vec3 camPosFrag;

// shadows 
uniform samplerCubeArray pointShadowMap;
in vec4 FragPosLightSpaceDir;
uniform sampler2D dirShadowMap;
int pointShadowCasterIndex = 0;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float DistributionGGX(vec3 N, vec3 H, float roughness);
vec3 CalcPointLight(Light light, vec3 N, vec3 V, vec3 F0);
//vec3 CalcDirLight(Light light, vec3 N, vec3 V, vec3 F0);
//vec3 CalcSpotLight(Light light, vec3 N, vec3 V, vec3 F0);
float PointShadowCalculation(vec3 fragPos, Light light, int shadowCasterIndex);
float PointShadowCalculation(vec3 fragPos, Light light, int shadowCasterIndex);
float DirShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
vec3 CalcDirLight(Light light, vec3 N, vec3 V, vec3 F0);
vec3 getNormalFromMap();

vec3 albedo;
vec3 emission;
float roughness; 
float metallic;
float ao;		
vec3 F0;

vec3 scaleAndBias(vec3 p) { return 0.5f * p + vec3(0.5f); }

bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }

vec3 fragPosUnscaled;

void main(){

	if(!isInsideCube(FragPos, 0)) return;
	scaledTexCoords = TexCoords;

	// Remember that we scaled some range of fragment positions to be inside clipspace in the vertex shader.
	// We have to unscale them to do the light calculations correctly. We are bringing FragPos from clip space to world space
	// to calculate lighting in world space.
	fragPosUnscaled = FragPos * voxelizationAreaSizeFrag;

	vec4 albedoSample = texture(material.texture_albedo1, scaledTexCoords).rgba;
	vec3 RMA = texture(material.texture_rma1, scaledTexCoords).gbr;
	emission = texture(material.texture_emission1, scaledTexCoords).rgb;

	roughness = RMA.r;
	metallic = RMA.g;

	albedo = albedoSample.rgb;

	vec3 N = getNormalFromMap();
	vec3 V = normalize(camPosFrag - fragPosUnscaled); // viewDir
	vec3 R = reflect(-V, N);   

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0); 

	// Calculate diffuse lighting fragment contribution.

	// ------ Directional Light ---------
	for(int i = 0; i < lights.length(); i++) {
		if (lights[i].type == 0.0)
		{
			Lo += CalcDirLight(lights[i], N, V, F0);
		}
	}

	// ------ Point lights ---------
	for(int i = 0; i < lights.length(); i++) {
		
		if (lights[i].type == 1.0)
		{
			Lo += CalcPointLight(lights[i], N, V, F0);
		}
	}

	// Output lighting to 3D texture.
	vec3 voxel = scaleAndBias(FragPos);
	ivec3 dim = imageSize(texture3D);	
	ivec3 voxelCoord = ivec3(dim * voxel);
	f16vec4 res = f16vec4(Lo, 1.0);

	imageAtomicMax(texture3D, voxelCoord, res);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom  * denom;

	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 CalcPointLight(Light light, vec3 N, vec3 V, vec3 F0) 
{
	float shadow = 0;

	if (light.shadowCaster.x == true) 
	{
		shadow = PointShadowCalculation(fragPosUnscaled, light, pointShadowCasterIndex);
		pointShadowCasterIndex++;
	}

	vec3 L = normalize(light.position.xyz - fragPosUnscaled); // lightDir
	vec3 H = normalize(V + L); // halfway vector

	//float distance = length(light.position.xyz - fragPosUnscaled);
	float attenuation = smoothstep(light.radius, 0.0, length(light.position.xyz - fragPosUnscaled));
	vec3 radiance = light.color.xyz * attenuation; // the scaling by the angle between the normal of the surface and the solid angle (which is just the direction vector
	// of the fragment to the light in this case) is in the final reflectance formula.

	//float NDF = DistributionGGX(N, H, roughness);
	//float G = GeometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

	// Cook-Torrance BRDF --> gives us how much each individual light ray (direction vector from fragment to light in this case) contributes 
	// to the final refleted light of an opaque surface (the current fragment) given its material properties.
	//vec3 numerator = NDF * G * F;
	//float denominator = 4.0 * max(dot(N, V), 0.0) *  max(dot(N, L), 0.0) + 0.0001;
	
	//vec3 specular = numerator / denominator;

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;

	// metallic surfaces don't refract light, so they have no diffuse reflections
	kD *= 1.0 - metallic;

	float NdotL = max(dot(N, L), 0.0); // scale the light's contribution by its angle to the surface's normal.

	//vec3 

	return (kD * albedo / PI) * radiance * NdotL * clamp((1.0 - shadow), 0.0, 1.0);
}

float PointShadowCalculation(vec3 fragPos, Light light, int shadowCasterIndex) 
{
	// get a vector from the light source to the current fragment so that we can (1) get its length to use as the depth value of the current fragment (remember that we used the length
	// of a vector from the light to a fragment as the depth value when generating the shadow map to make the depth values linear. We use perspective projection when rendering the scene
	// to create point shadow map, so the depth values stored would not be linear, and that is why we had to do it ourselves.
	// (2) we also use this vector to sample a depth value from the shadow cubemap.

	float shadow = 0.0;

	vec3 fragToLight = fragPos - light.position.xyz;

	float currentDepth = length(fragToLight);
	
	float closestDepth = texture(pointShadowMap, vec4(fragToLight, shadowCasterIndex)).r;
	closestDepth *= light.radius;   // undo mapping [0;1]
	
	if(currentDepth > closestDepth)
	{
		shadow = 1.0;
	}

	return shadow;
}	


vec3 CalcDirLight(Light light, vec3 N, vec3 V, vec3 F0) 
{

	// same thing as point light calculation, but the light direction L does not depend on the light position. It is a constant direciton.
	// There is also no attenuation

	vec3 L = normalize(light.position.xyz); // lightDir

	float shadow = 0;

	if (light.shadowCaster.x == true) 
	{
		shadow = DirShadowCalculation(FragPosLightSpaceDir, N, L);
	}

	vec3 H = normalize(V + L); // halfway vector

	vec3 radiance = light.color.xyz;

	//float NDF = DistributionGGX(N, H, roughness);
	//float G = GeometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

	//vec3 numerator = NDF * G * F;
	//float denominator = 4.0 * max(dot(N, V), 0.0) *  max(dot(N, L), 0.0) + 0.0001;
		
	//vec3 specular = numerator / denominator;

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;

	kD *= 1.0 - metallic;

	float NdotL = max(dot(N, L), 0.0); // scale the light's contribution by its angle to the surface's normal.
		
	return (kD * albedo / PI) * radiance * NdotL * clamp((1.0 - shadow), 0.0, 1.0);
}

float DirShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
	
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; // this returns the fragment's light space position in the range [-1, 1]

	projCoords = projCoords * 0.5 + 0.5;

	float currentDepth = projCoords.z;

	float shadow = 0.0;

	float closestDepth = texture(dirShadowMap, projCoords.xy).r; 
	shadow += currentDepth > closestDepth ? 1.0 : 0.0;        

	if(projCoords.z > 1.0)
        shadow = 0.0;

	return shadow;
}

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(material.texture_normal1, scaledTexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}