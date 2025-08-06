#version 460 core
layout(quads, equal_spacing, ccw) in; // Correct input primitive

out vec2 UV;
out vec3 N;
out vec3 FragPos;
out vec4 viewFragPos;

in vec2 uvsCoord[];


uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform mat4 P;
uniform mat3 normalMatrix3;

uniform float time;
uniform vec3 randomDir[32];

vec2 windDir = normalize(vec2(0.5,0.4));
float windSpeed = 1.0;

float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*43758.5453123);
}

float CircularWave(vec2 pos, vec2 source, float time, float A, float k, float omega, float phi) {
    float r = length(pos - source);
    return A * cos(k * r - omega * time + phi) / sqrt(r + 0.001); // avoid divide-by-zero
}

const int NUM_WAVES = 3;
vec2 waveSources[NUM_WAVES] = vec2[](
    vec2(-12, -4.5),
    vec2(-2.0, 3.0),
    vec2(0.0, -1.0)
);




void main() {
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // Interpolate UVs
    vec2 uv0 = uvsCoord[0];
    vec2 uv1 = uvsCoord[1];
    vec2 uv2 = uvsCoord[2];
    vec2 uv3 = uvsCoord[3];

    vec2 leftUV = mix(uv0, uv3, v);
    vec2 rightUV = mix(uv1, uv2, v);
    UV = mix(leftUV, rightUV, u);

    // Interpolate positions (use XYZ to ignore W)
    vec3 pos0 = gl_in[0].gl_Position.xyz;
    vec3 pos1 = gl_in[1].gl_Position.xyz;
    vec3 pos3 = gl_in[3].gl_Position.xyz;
    vec3 pos2 = gl_in[2].gl_Position.xyz;

    vec3 leftPos = mix(pos0, pos3, v);
    vec3 rightPos = mix(pos1, pos2, v);
    vec3 pos = mix(leftPos, rightPos, u);
    pos.y += 1;


    FragPos = pos;
    viewFragPos = V * vec4(pos, 1.0);
    gl_Position = P * V * vec4(pos, 1.0); // Add model-view-projection matrix here
}