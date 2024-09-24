#version 330 core
layout (location = 1) out vec4 gNormalBuffer;
layout (location = 2) out vec4 gAlbedoSpec;  // Stores both albedo and specular in one vector


in vec4 posFS;
in vec4 posW;
in vec2 uvFS;

uniform sampler2D gPostion;    // View-space position
uniform sampler2D gAlbeido;
uniform sampler2D gNormal;
uniform sampler2D gDepth;

uniform sampler2D decalTexture;
uniform sampler2D decalNormal;

uniform mat4 V; // View matrix
uniform mat4 M; // Model matrix
uniform mat4 P; // Projection matrix
uniform mat4 inverseV;
uniform mat4 inverseP;
uniform mat4 inverseM;


uniform vec2 resolution;
uniform vec3 size;
in vec2 TexCoords;
in vec3 DecalCenterPosition;


void main () {


    vec2 depthCoords = gl_FragCoord.xy / vec2(resolution.x, resolution.y);
    float z = texture(gDepth, vec2(depthCoords.s, depthCoords.t)).x * 2.0f - 1.0f;

    vec4 clipSpacePosition = vec4(vec2(depthCoords.s, depthCoords.t) * 2.0 - 1.0, z, 1.0);

    //clipSpacePosition = vec4(vec2(depthCoords.s * 2.0 - 1.0, ((depthCoords.t * 2 + -1) * 2.0 - 1.0) ), z, 1.0);
    vec4 viewSpacePosition = inverseP * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = inverseV * viewSpacePosition;
    vec3 FragPos = worldSpacePosition.xyz;

    vec4 localpos = inverseM * vec4(FragPos,1);
     
    float d = length(FragPos - DecalCenterPosition);

	if (d > size.x || d > size.z)	
		discard;
    else{
        // Calculate texture coordinates, adjusting for aspect ratio
        vec2 textureCoordinate = (localpos.xz) + 0.5;
        // Clamp to [0, 1] to avoid repetition
        textureCoordinate = clamp(textureCoordinate, 0.0, 1.0);
        vec4 gBaseColor = texture( decalTexture, vec2(textureCoordinate.x, textureCoordinate.y));
        //gBaseColor = vec4( ,1);
         gAlbedoSpec = gBaseColor;
     }
}