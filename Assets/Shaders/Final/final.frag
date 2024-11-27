#version 430 core
out vec3 FragColor;

in vec2 UV;
uniform sampler2D gFinal; 
void main() {
    // Retrieve data from G-buffer
    vec3 final = texture(gFinal, UV).rgb;
    FragColor = final;
}
