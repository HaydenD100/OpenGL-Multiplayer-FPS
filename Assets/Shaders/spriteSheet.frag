#version 460 core
layout (location = 2) out vec4 FragOut;
layout (location = 5) out vec4 gEmission;  // Stores both albedo and specular in one vector

layout (binding = 0) uniform sampler2D Texture;

in vec2 TexCoord;
in vec2 TexCoordNext;
uniform float u_mixFactor;
uniform bool IsEmissive;

void main() {
    vec4 color = texture2D(Texture, TexCoord);
    vec4 colorNext = texture2D(Texture, TexCoordNext);
    FragOut = mix(color, colorNext, u_mixFactor);
    if(IsEmissive)  
        gEmission = mix(color, colorNext, u_mixFactor);
}