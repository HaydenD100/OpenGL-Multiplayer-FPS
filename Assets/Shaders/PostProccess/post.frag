#version 430 core
layout (location = 0) out vec4 gFinal;

in vec2 UV;
uniform sampler2D gLighting; 
uniform sampler2D gSSR; 
layout(binding = 2) uniform sampler2D gEmissive; 


vec4 center;
vec4 boxBlur2D(sampler2D image, vec2 texCoords, vec2 texOffset, int kernelSize)
{
    vec4 result = vec4(0.0);
    int kernelRadius = kernelSize / 2; // Radius of the kernel
    int sampleCount = kernelSize * kernelSize; // Total number of samples

    // Loop over the kernel in both x and y directions
    for (int x = -kernelRadius; x <= kernelRadius; ++x)
    {
        for (int y = -kernelRadius; y <= kernelRadius; ++y)
        {
            // Sample the texture at the offset position
            result += texture(image, texCoords + vec2(texOffset.x * x, texOffset.y * y));
        }
    }

    // Normalize by the number of samples
    result /= float(sampleCount);

    return result;
}

void main() {

    // Retrieve data from G-buffer
    vec4 lighting = vec4(texture(gLighting, UV).rgb, 1.0);
    vec4 ssr = texture(gSSR, UV).rgba;
    vec2 texSize  = textureSize(gSSR, 0).xy;
    vec4 emissive = vec4(texture(gEmissive, UV).rgb,1); 

    /*
    vec2 texOffset = 1.0 / textureSize(gSSR, 0); // Texture offset
    bool horizontal = false;
    int kernelSize = 5; // Kernel size (e.g., 5x5)

    vec4 blurredSSR = ssr;
    //if(ssr.a > 0.001f && ssr.r > 0.001f && ssr.g > 0.001f && ssr.b > 0.001f)
        //blurredSSR = boxBlur2D(gSSR, UV, texOffset, kernelSize);
    */

    //+ blurredSSR
    gFinal = lighting  + emissive ;

}
