#version 330 core

in vec2 UV;
#define MAXLIGHTS 10

out vec4 color;

uniform sampler2D gPostion;
uniform sampler2D gNormal;
uniform sampler2D gAlbeido;
uniform sampler2D depthTexture;

uniform vec3 LightColors[MAXLIGHTS]; // Array of light colors
uniform vec3 LightPositions_worldspace[MAXLIGHTS]; // Array of light positions
uniform float LightLinears[MAXLIGHTS];
uniform float LightQuadratics[MAXLIGHTS];

uniform vec3 viewPos;

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPostion, UV).rgb;
    vec3 Normal = texture(gNormal, UV).rgb;
    vec3 Diffuse = texture(gAlbeido, UV).rgb;
    float Specular = texture(gAlbeido, UV).a;
    
    // then calculate lighting as usual
   
    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    for(int i = 0; i < MAXLIGHTS; ++i)
    {
        // calculate distance between light source and current fragment
        float distance = length(LightPositions_worldspace[i] - FragPos);
        if(distance < 15)
        {
            // diffuse
            vec3 lightDir = normalize(LightPositions_worldspace[i] - FragPos);
            vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * LightColors[i];
            // specular
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
            vec3 specular = LightColors[i] * spec * 0.5;
            // attenuation
            float attenuation = 1.0 / (1.0 + LightLinears[i] * distance + LightQuadratics[i] * distance * distance);
            diffuse *= attenuation;
            specular *= attenuation;
            lighting += diffuse + specular;
        }
    }    
    color = vec4(lighting, 1.0);
}