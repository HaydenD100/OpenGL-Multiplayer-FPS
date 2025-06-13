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

vec3 BrownianMotion(vec3 pos) {
    float f = 1.0;
    float a = 1.0;
    float speed = 0.8;
    float seed = 0.0;
    vec3 p = pos;
    float amplitudeSum = 0;
    float h = 0.0;
    vec2 derivatives = vec2(0.0); // Stores (dh/dx, dh/dz)
    float maxPeak = 0.9;

    for (int i = 0; i < 64; i++) {
        vec2 dir = normalize(mix(vec2(cos(seed), sin(seed)),windDir,0.3)); // Wave direction
        float x = dot(dir, p.xz) * f + time * speed; // Scaled position + time
        float wave = a * exp(maxPeak * sin(x) - 1.0); // Wave height
        float dwave_dx = f * wave * cos(x); // Derivative INCLUDES frequency (f)

        h += wave * 0.5;
        p.xz += dir * -dwave_dx * 0.1;
        derivatives += dir * dwave_dx; // Accumulate x/z derivatives

        amplitudeSum += a;
        //p.xz  += (dir * dwave_dx) / amplitudeSum;

        f *= 1.18; // Lacunarity
        a *= 0.79; // Gain
        speed *= 1.08;
        seed += 1253.2131;
    }



    // Normalize height and derivatives
    h /= amplitudeSum * 1.0;
    derivatives /= amplitudeSum;

     vec3 T = normalize(vec3(1.0, derivatives.x, 0.0)); // Tangent (x-axis)
     vec3 B = normalize(vec3(0.0, derivatives.y, 1.0)); // Bitangent (z-axis)
     N = normalize(cross(B, T)); // Final normal (order matters!)

    // Incorporate derivatives for horizontal displacement
    return vec3(derivatives.x * 0.1, h, derivatives.y * 0.1); // Scale the displacement as needed
}


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
    pos += BrownianMotion(pos);


    FragPos = pos;
    viewFragPos = V * vec4(pos, 1.0);
    gl_Position = P * V * vec4(pos, 1.0); // Add model-view-projection matrix here
}