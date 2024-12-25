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
    vec4 blended = lighting;

    vec4 emissive = vec4(texture(gEmissive, UV).rgb,1); 
    
    // Initialize the final color and weight sum
    vec4 color = vec4(0.0);
    float totalWeight = 0.0;

     
     vec4 emissiveBlurred;

     if (emissive.xyz != vec3(0,0,0)) { 
         vec4 center =  texture(gEmissive, clamp(UV, 0,1));
        // Perform a 9x9 kernel pass (from -4 to 4 in both directions)
        for (float x = -5.0; x <= 5.0; x += 1.0) {
            for (float y = -5.0; y <= 5.0; y += 1.0) {

                vec4 sample0 = texture(gEmissive, clamp(UV + vec2(x, y) / texSize , 0,1));
                // Weight based on similarity to the center value (e.g., current pixel SSR value)
                float weight = 1.0 - abs(dot(sample0.rgb - center.rgb, vec3(0.25)));
                weight = pow(weight, 0.1); // Adjust exponent to control blending sharpness
                color += sample0 * weight;
                totalWeight += weight;
            }
        }
        // Average the color based on total weight
        color /= totalWeight;

       emissiveBlurred = color;
    }


    /*
    const vec2 texelSize = vec2(1.0) / textureSize(emissiveBlurred, 0);

    vec4 color1 = vec4(0.0);
    for (int i = 0; i < gaussKernel3x3.length(); ++i)
        color += gaussKernel3x3[i].w * texture(emissiveBlurred, UV + texelSize * gaussKernel3x3[i].xy);
    */
    
    //gFinal = ssrBlurred;

    // Final output to the screen
    //gFinal = emissiveBlurred;
    gFinal = blended + ssr + emissiveBlurred;

}
