#version 330 core
#define MAXLIGHTS 10

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

in vec3 LightDirection_tangentspace;
in vec3 EyeDirection_tangentspace;

// Output data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D DiffuseTextureSampler;
uniform sampler2D NormalTextureSampler;
uniform vec3 LightColors[MAXLIGHTS]; // Array of light colors
uniform vec3 LightPositions_worldspace[MAXLIGHTS]; // Array of light positions
uniform float LightConstants[MAXLIGHTS];
uniform float LightLinears[MAXLIGHTS];
uniform float LightQuadratics[MAXLIGHTS];



float LinearizeDepth(float depth) 
{
    float near = 0.1;
    float far = 100;
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}


void main() {
    // Material properties
    vec3 MaterialDiffuseColor = texture(DiffuseTextureSampler, UV).rgb;
    vec3 MaterialAmbientColor = vec3(0.1) * MaterialDiffuseColor;
    vec3 MaterialSpecularColor = vec3(0.0);

    // Initialize final color
    vec3 FinalColor = vec3(0,0,0);

    // Calculate lighting contributions from each light
    for (int i = 0; i < MAXLIGHTS; ++i) {
        //it kept stacking up the ambient light if there were no lights so this prevents in from adding if 
        if(LightConstants[i] == 0)
            continue;

        // Local normal, in tangent space. V tex coordinate is inverted because normal map is in TGA (not in DDS) for better quality
        vec3 TextureNormal_tangentspace = normalize(texture(NormalTextureSampler, UV).rgb * 2.0 - 1.0);

        // Distance to the light
        float distance = length(LightPositions_worldspace[i] - Position_worldspace);
        float attenuation = 1.0 / (LightConstants[i] + LightLinears[i] * distance + LightQuadratics[i] * (distance * distance));    


        // Normal of the computed fragment, in camera space
        vec3 n = TextureNormal_tangentspace;
        // Direction of the light (from the fragment to the light)
        vec3 l = normalize(LightDirection_tangentspace);
        float cosTheta = clamp(dot(n, l), 0.0, 1.0);

        // Eye vector (towards the camera)
        vec3 E = normalize(EyeDirection_tangentspace);
        // Direction in which the triangle reflects the light
        vec3 R = reflect(-l, n);
        // Cosine of the angle between the Eye vector and the Reflect vector,
        // clamped to 0
        //  - Looking into the reflection -> 1
        //  - Looking elsewhere -> < 1
        float cosAlpha = clamp(dot(E, R), 0.0, 1.0);
        
        FinalColor +=
            // Ambient : simulates indirect lighting
            MaterialAmbientColor +
            // Diffuse : "color" of the object
            MaterialDiffuseColor * LightColors[i] * attenuation;
            // Specular : reflective highlight, like a mirror
            MaterialSpecularColor * LightColors[i] * attenuation;
    }


    //this will make things far away darker
    float depth = 1 - LinearizeDepth(gl_FragCoord.z) / 100; // divide by far for demonstration

    //color = vec4(vec3(depth), 1.0);

    // Output final color
    color = vec4(FinalColor, texture(DiffuseTextureSampler, UV).a) * vec4(vec3(depth), 1.0);
}