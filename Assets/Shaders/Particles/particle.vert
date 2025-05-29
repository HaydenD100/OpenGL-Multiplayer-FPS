#version 430

layout(location = 0) in vec2 quadVertex; // e.g. -0.5,-0.5 ... 0.5,0.5

struct Particle {
    vec3 position;
    vec3 velocity;
    float lifetime;
};

layout(std430, binding = 0) readonly buffer Particles {
    Particle particles[];
};

uniform mat4 V;
uniform mat4 P;
uniform float size;

out vec2 uv;

void main() {
    uint instanceID = gl_InstanceID;
    Particle p = particles[instanceID];

    // Get camera-facing right and up vectors
    vec3 right = vec3(V[0][0], V[1][0], V[2][0]);
    vec3 up    = vec3(V[0][1], V[1][1], V[2][1]);

    vec3 worldPos = p.position + (right * quadVertex.x + up * quadVertex.y) * size;

    gl_Position = P * V * vec4(worldPos, 1.0);
    uv = quadVertex + 0.5; // Map [-0.5, 0.5] -> [0, 1]
}