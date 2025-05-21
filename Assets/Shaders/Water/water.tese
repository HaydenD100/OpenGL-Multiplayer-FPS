#version 460 core
layout(quads, equal_spacing, ccw) in; // Correct input primitive

out vec2 UV;
out vec3 N;
out vec3 FragPos;
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

vec3 BrownianMotion(vec3 pos) {
    float f = 1.0;
    float a = 1.0;
    float speed = 1.0;
    float seed = 0.0;
    vec3 p = pos;
    float amplitudeSum = 0;
    float h = 0.0;
    vec2 derivatives = vec2(0.0); // Stores (dh/dx, dh/dz)
    float maxPeak = 1.5;
    //h += 0.5 * sin(dot(pos.xz,windDir) + time * windSpeed);


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

/*
vec3 BrownianMotion(vec3 pos) {
	float f = 1;
	float a = 1;
	float speed = 2;
	float seed = 0;
	vec3 p = pos;
	float amplitudeSum = 0.0f;

    float max = 1;

    float h = 0.0f;
	vec2 n = vec2(0.0f);

    for (int i = 0; i < 32; i++) {
        vec2 d = normalize(vec2(cos(seed), sin(seed)));
        float x = dot(d, p.xz) * f + time * speed;
		float wave = a * exp(1 * sin(x) - 1);
        float dx = 1 * wave * cos(x);

        h += wave;		
		p.xz += d * -dx * a * 1;


        amplitudeSum += a;
		f *= 1.18;
		a *= 0.82;
		speed *= 1.07;
		seed += 1253.2131f;
    }
    vec3 outputPos = vec3( p.x, h, p.z) / amplitudeSum;
	outputPos.y *= 1;

    vec3 T = normalize(vec3(1.0, p.x, 0.0)); // Tangent (x-axis)
    vec3 B = normalize(vec3(0.0, p.z, 1.0)); // Bitangent (z-axis)
    N = normalize(cross(B, T)); // Final normal (order matters!)


    return outputPos;
}

*/


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
    gl_Position = P * V * vec4(pos, 1.0); // Add model-view-projection matrix here
}