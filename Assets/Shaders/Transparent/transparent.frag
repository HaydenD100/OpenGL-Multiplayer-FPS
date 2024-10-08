#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;  // Stores both albedo and specular in one vector

in vec2 UV;

uniform sampler2D gAlbeido;
uniform sampler2D gPositionTexture;

uniform sampler2D DiffuseTextureSampler;

void main()
{    

    float alpha = texture(DiffuseTextureSampler, UV).a;
    vec3 MaterialDiffuseColor = texture(DiffuseTextureSampler, UV).rgb;
    gAlbedoSpec = vec4(MaterialDiffuseColor, alpha); // RGB for Albedo, R for Specular Intensity
}