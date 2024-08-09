#version 330 core
#define MAXLIGHTS 10

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexTangent_modelspace;
layout(location = 4) in vec3 vertexBitangent_modelspace;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 Position_worldspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

out vec3 LightDirection_tangentspace;
out vec3 EyeDirection_tangentspace;

out vec4 FragPosLightSpace;


// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform mat3 MV3x3;
uniform vec3 LightPositions_worldspace[MAXLIGHTS]; // Array of light positions
uniform mat4 lightSpaceMatrix;

void main() {
    // Output position of the vertex, in clip space : MVP * position
    gl_Position =  MVP * vec4(vertexPosition_modelspace, 1.0);
    
    // Position of the vertex, in worldspace : M * position
    Position_worldspace = (M * vec4(vertexPosition_modelspace, 1.0)).xyz;
    
    // Vector that goes from the vertex to the camera, in camera space.
    // In camera space, the camera is at the origin (0,0,0).
    vec3 vertexPosition_cameraspace = (V * M * vec4(vertexPosition_modelspace, 1.0)).xyz; 
    EyeDirection_cameraspace = vec3(0.0, 0.0, 0.0) - vertexPosition_cameraspace;

    // Calculate light directions and transform to tangent space
    for (int i = 0; i < MAXLIGHTS; ++i) {
        vec3 LightPosition_cameraspace = (V * vec4(LightPositions_worldspace[i], 1.0)).xyz;
        LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

        // model to camera = ModelView
        vec3 vertexTangent_cameraspace = MV3x3 * vertexTangent_modelspace;
        vec3 vertexBitangent_cameraspace = MV3x3 * vertexBitangent_modelspace;
        vec3 vertexNormal_cameraspace = MV3x3 * vertexNormal_modelspace;
        
        mat3 TBN = transpose(mat3(
            vertexTangent_cameraspace,
            vertexBitangent_cameraspace,
            vertexNormal_cameraspace   
        ));

        LightDirection_tangentspace = TBN * LightDirection_cameraspace;
        EyeDirection_tangentspace = TBN * EyeDirection_cameraspace;

        FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

        // Send outputs for interpolation
        UV = vertexUV;
    }
}