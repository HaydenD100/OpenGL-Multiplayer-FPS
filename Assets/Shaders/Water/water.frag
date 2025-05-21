#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;  // Stores both albedo and specular in one vector
layout (location = 3) out vec4 gRMA;  // Stores both albedo and specular in one vector
layout (location = 4) out vec4 gTrueNormal;  // Stores both albedo and specular in one vector
layout (location = 5) out vec4 gEmission;  // Stores both albedo and specular in one vector

in vec2 UV;
in vec3 FragPos;
in vec3 N;

layout(binding = 0) uniform samplerCube sky;    // View-space position

uniform vec3 viewpos;
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform mat4 P;


void main()
{    
    vec3 waterColor = vec3(0, 0.4, 0.6);
    //
    gPosition = vec3(V * vec4(FragPos,1));
    // also store the per-fragment normals into the gbuffer
    vec3 I = normalize(viewpos - FragPos);
    vec3 R = reflect(I, normalize(N));
    vec3 envColor = texture(sky, R).rgb * 0.2;
    float fresnel = pow(1.0 - dot(normalize(N), -I), 5.0);
    fresnel = clamp(fresnel, 0.0, 1.0);
    //fresnel = clamp(fresnel, 0.0, 1.0);

    gNormal = V * vec4(N, 0);
    //vec3 directLighting = DirectLighting();
    vec3 color = mix(waterColor, envColor , fresnel);

    //color = waterColor;
    // w = ambient strenght
    gRMA = vec4(0.04,0.0,0,10);
    gAlbedo =  vec4(color,1);
    gTrueNormal = V * vec4(N,0);  // Use transpose of TBN to inverse the transformation
}