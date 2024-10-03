#version 330 core
layout (location = 1) out vec4 gNormalBuffer;
layout (location = 2) out vec4 gAlbedoSpec;  // Stores both albedo and specular in one vector

in vec3 DecalCenterPosition;
in mat4 inverseM;

uniform sampler2D gDepth;  // Depth buffer (remove unused uniforms)
uniform sampler2D decalTexture;

uniform mat4 inverseP;
uniform mat4 inverseV;

uniform vec2 resolution;
uniform vec3 size;

void main() {

    // Normalize depth texture coordinates
    vec2 depthCoords = gl_FragCoord.xy / resolution;
    // Fetch the depth value and reconstruct clip-space position
    float z = texture(gDepth, depthCoords).x * 2.0 - 1.0;
    vec4 clipSpacePosition = vec4(depthCoords * 2.0 - 1.0, z, 1.0);
    
    // Convert to view-space and then to world-space position
    vec4 viewSpacePosition = inverseP * clipSpacePosition;
    vec4 worldSpacePosition = inverseV * (viewSpacePosition /= viewSpacePosition.w);
    vec3 FragPos = worldSpacePosition.xyz;
    // Calculate the distance from the decal center and discard if out of bounds
    float d = length(FragPos - DecalCenterPosition);
    if (d > size.x || d > size.z)
        discard;
    else{
        // Convert to local space for decal operations
        vec4 localPos = inverseM * vec4(FragPos, 1.0);
        // Compute texture coordinates in local space (XZ plane)
        vec2 textureCoordinate = (localPos.xz * 0.5) + 0.5;
        textureCoordinate = clamp(textureCoordinate, 0.0, 1.0);
        // Sample and assign decal texture
        vec4 decalTex = texture(decalTexture, textureCoordinate);
        gAlbedoSpec = decalTex;
        //add normal
        //if(decalTex.a == 0){
            //gNormalBuffer = texture(decalTexture, textureCoordinate);
        //}

    }
}