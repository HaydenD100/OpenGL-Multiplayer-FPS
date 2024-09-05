#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;  // Stores both albedo and specular in one vector

in vec2 UV;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

uniform sampler2D gAlbeido;
uniform sampler2D gPositionTexture;


uniform sampler2D DiffuseTextureSampler;
uniform sampler2D NormalTextureSampler;

void main()
{    
    float alpha = texture(DiffuseTextureSampler, UV).a;
    vec3 MaterialDiffuseColor = texture(DiffuseTextureSampler, UV).rgb;
    //if(texture(gPositionTexture, vec2(gl_FragCoord)).z < gl_FragDepth)
         //alpha = 0;

    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    gAlbedoSpec = vec4(MaterialDiffuseColor, alpha); // RGB for Albedo, R for Specular Intensity

}