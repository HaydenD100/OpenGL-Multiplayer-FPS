#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedoSpec;  // Stores both albedo and specular in one vector


in vec2 UV;
in vec3 normal;
in vec3 Position_worldspace;

uniform sampler2D DiffuseTextureSampler;
uniform sampler2D NormalTextureSampler;
uniform sampler2D SpecularColorTextureSampler;

void main()
{    
    vec3 MaterialDiffuseColor = texture(DiffuseTextureSampler, UV).rgb;
    vec3 MaterialSpecularColor = texture(SpecularColorTextureSampler, UV).rgb;

    gPosition = Position_worldspace;
    gNormal = normal;
    gAlbedoSpec = MaterialDiffuseColor.rgb; // RGB for Albedo, R for Specular Intensity
}