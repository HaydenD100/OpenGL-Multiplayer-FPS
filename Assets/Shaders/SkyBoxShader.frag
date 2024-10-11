#version 430 core
layout (location = 2) out vec4 gAlbedo; 
layout (location = 3) out vec4 gPBR;  // Stores both albedo and specular in one vector

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    gPBR = vec4(0,0,1,0);
    gAlbedo = texture(skybox, TexCoords);
}