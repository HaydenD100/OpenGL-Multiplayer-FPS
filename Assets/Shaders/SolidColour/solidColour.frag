#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;  // Stores both albedo and specular in one vector
layout (location = 3) out vec4 gRMA;  // Stores both albedo and specular in one vector
layout (location = 4) out vec4 gTrueNormal;  // Stores both albedo and specular in one vector
layout (location = 5) out vec4 gEmission;  // Stores both albedo and specular in one vector


in vec2 UV;
in vec3 FragPos;
in vec3 TrueNormal;



uniform mat4 V;

uniform bool IsEmissive;
uniform vec3 color;
uniform float Roughness;
uniform float Metalic;


void main()
{    


    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    gRMA = vec4(Roughness,Metalic,0,0);
    gNormal = vec4(TrueNormal, 0);
    gAlbedo = vec4(color, 1); // RGB for Albedo, R for Specular Intensity
    gTrueNormal = vec4(TrueNormal,0);  // Use transpose of TBN to inverse the transformation
    if(IsEmissive){
        gEmission = vec4(color, 1);
    }
        

}