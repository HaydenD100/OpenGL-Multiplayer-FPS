#version 330 core

in vec2 UV;
#define MAXLIGHTS 60

out vec4 color;

uniform sampler2D gPostion;    // View-space position
uniform sampler2D gNormal;     // View-space normal
uniform sampler2D gAlbeido;
uniform sampler2D ssaoTexture;

uniform vec3 LightColors[MAXLIGHTS];
uniform vec3 LightPositions_worldspace[MAXLIGHTS];
uniform float LightLinears[MAXLIGHTS];
uniform float LightQuadratics[MAXLIGHTS];
uniform float LightRadius[MAXLIGHTS];

uniform vec3 viewPos;
uniform mat4 inverseV; // Inverse of the view matrix
uniform mat4 V; // TT

void main()
{
    // Retrieve data from G-buffer
    vec3 FragPos_view = vec3(texture(gPostion, UV).rgb); // View-space position
    vec3 Normal_view = normalize(vec3(texture(gNormal, UV).rgb)); // View-space normal
    vec3 Diffuse = texture(gAlbeido, UV).rgb;
    float Specular = texture(gAlbeido, UV).a;
    float AmbientOcclusion = texture(ssaoTexture, UV).r;

    // Transform view-space position to world-space position
    vec4 FragPos_world = inverseV * vec4(FragPos_view, 1.0);
    FragPos_world /= FragPos_world.w; // Normalize by w
    vec3 FragPos = FragPos_world.xyz;
    // Transform view-space normal to world-space normal
    vec3 Normal_world = normalize((transpose(V) * vec4(Normal_view, 0.0)).xyz);

    // Calculate ambient lighting
    vec3 ambient = vec3(0.15 * Diffuse * AmbientOcclusion);

    // Lighting calculations
    vec3 lighting = ambient;
    vec3 viewDir = normalize(viewPos - FragPos);

    for(int i = 0; i < MAXLIGHTS; ++i)
    {
        if(LightRadius[i] == 0)
            continue;
        // Calculate distance between light source and current fragment
        float distance = length(LightPositions_worldspace[i] - FragPos);
        if(distance < LightRadius[i])
        {
            // Diffuse lighting
            vec3 lightDir = normalize(LightPositions_worldspace[i] - FragPos);
            vec3 diffuse = max(dot(Normal_world, lightDir), 0.0) * Diffuse * LightColors[i];

            // Specular lighting
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(Normal_world, halfwayDir), 0.0), 16.0);
            vec3 specular = LightColors[i] * spec * Specular;

            // Attenuation
            float attenuation = 1.0 / (1.0 + LightLinears[i] * distance + LightQuadratics[i] * distance * distance);
            diffuse *= attenuation;
            specular *= attenuation;

            lighting += diffuse + specular;
        }
    }

    //not sure if this all works


    float gamma = 1;
    float exposure = 1.0;
  
    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-lighting * exposure);
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    color = vec4(mapped, 1.0);
}