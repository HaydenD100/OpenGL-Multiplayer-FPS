#version 430 core
layout (location = 0) out vec4 gFinal;

in vec2 UV;
uniform sampler2D gLighting; 
uniform sampler2D gSSR; 
layout(binding = 2) uniform sampler2D gEmissive; 


vec4 center;

void main() {

    // Retrieve data from G-buffer
    vec4 lighting = vec4(texture(gLighting, UV).rgb, 1.0);
    vec4 ssr = texture(gSSR, UV).rgba;
    vec2 texSize  = textureSize(gSSR, 0).xy;
    vec4 emissive = vec4(texture(gEmissive, UV).rgb,1); 
    
    gFinal = lighting  + ssr + emissive ;

}
