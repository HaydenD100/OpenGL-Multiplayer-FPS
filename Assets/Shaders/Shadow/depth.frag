#version 330 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;


uniform sampler2D DiffuseTextureSampler;
uniform sampler2D NormalTextureSampler;
uniform sampler2D RoughnessTextureSampler;
uniform sampler2D MetalicTextureSampler;

uniform float Roughness;
uniform float Metalic;


void main()
{
    float lightDistance = length(FragPos.xyz - lightPos);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;
    // write this as modified depth
    //gl_FragDepth = lightDistance;
    gl_FragDepth = 0.5;

}