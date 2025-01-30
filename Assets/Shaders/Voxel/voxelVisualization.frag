#version 460 core

in vec4 fragColor;  // Input color from the vertex shader

layout (location = 0) out vec4 FragColor; // Final color output

void main()
{  
    if(fragColor.a == 0.0)
    {
        discard;
    }
   
    FragColor = fragColor; // Set the color with full alpha
}