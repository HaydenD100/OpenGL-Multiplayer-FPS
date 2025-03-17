#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gRMA;
layout (location = 4) out vec4 gTrueNormal;
layout (location = 5) out vec4 gEmission;

uniform vec3 Colour;

void main()
{
    gAlbedo = vec4(Colour, 1.0); // Red color
}