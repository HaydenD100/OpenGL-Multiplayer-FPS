#version 430 core
out vec3 FragColor;
  
in vec2 UV;
  
uniform sampler2D ssaoTexture;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoTexture, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoTexture, UV + offset).r;
        }
    }
    FragColor = vec3(result / (4.0 * 4.0));
}  