#version 430 core
layout (location = 0) out vec4 gFinal;

in vec2 UV;
uniform sampler2D gLighting; 
uniform sampler2D gSSR; 


vec4 center; // Assuming this is the lighting or base color in your scene


void main() {
    // Retrieve data from G-buffer
    vec4 lighting = vec4(texture(gLighting, UV).rgb, 1.0);
    vec4 ssr = texture(gSSR, UV).rgba;
    vec2 texSize  = textureSize(gSSR, 0).xy;


    // Initialize the final color and weight sum
    vec4 color = vec4(0.0);
    float totalWeight = 0.0;

     vec4 blended = lighting;
     vec4 ssrBlurred;

     if (ssr.a > 0.0) { 
         float reflectionAlpha = ssr.a;
         vec4 center =  texture(gSSR, UV);
        // Perform a 9x9 kernel pass (from -4 to 4 in both directions)
        for (float x = -4.0; x <= 4.0; x += 1.0) {
            for (float y = -4.0; y <= 4.0; y += 1.0) {
                vec4 sample0 = texture(gSSR, UV + vec2(x, y) / texSize);
                // Weight based on similarity to the center value (e.g., current pixel SSR value)
                float weight = 1.0 - abs(dot(sample0.rgb - center.rgb, vec3(0.25)));
                weight = pow(weight, 3.0); // Adjust exponent to control blending sharpness
                color += sample0 * weight;
                totalWeight += weight;
            }
        }
        // Average the color based on total weight
        color /= totalWeight;

       ssrBlurred = color;
    }

    

    //gFinal = ssrBlurred;

    // Final output to the screen
    gFinal = blended + ssrBlurred;
}
