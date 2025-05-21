#version 460 core
layout (vertices = 4) out;

in vec2 UV[];
out vec2 uvsCoord[];

uniform vec3 cameraPosition;

const float MIN_TES = 16.0;
const float MAX_TES = 128.0; 
const float MIN_DIST = 10.0;
const float MAX_DIST = 40.0;

void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    uvsCoord[gl_InvocationID] = UV[gl_InvocationID];

    if (gl_InvocationID == 0) {
        // Calculate midpoints of quad edges (using XYZ to ignore W)
        vec3 center0 = 0.5 * (gl_in[0].gl_Position.xyz + gl_in[3].gl_Position.xyz); // Left edge
        vec3 center1 = 0.5 * (gl_in[1].gl_Position.xyz + gl_in[0].gl_Position.xyz); // Bottom edge
        vec3 center2 = 0.5 * (gl_in[2].gl_Position.xyz + gl_in[1].gl_Position.xyz); // Right edge
        vec3 center3 = 0.5 * (gl_in[3].gl_Position.xyz + gl_in[2].gl_Position.xyz); // Top edge

        // Calculate distances in the SAME coordinate system (e.g., world space)
        float dist0 = distance(cameraPosition.xyz, center0);
        float dist1 = distance(cameraPosition.xyz, center1);
        float dist2 = distance(cameraPosition.xyz, center2);
        float dist3 = distance(cameraPosition.xyz, center3);

        // Compute tessellation levels
        float tes0 = mix(MAX_TES, MIN_TES, clamp(dist0 / MAX_DIST, 0.0, 1.0));
        float tes1 = mix(MAX_TES, MIN_TES, clamp(dist1 / MAX_DIST, 0.0, 1.0));
        float tes2 = mix(MAX_TES, MIN_TES, clamp(dist2 / MAX_DIST, 0.0, 1.0));
        float tes3 = mix(MAX_TES, MIN_TES, clamp(dist3 / MAX_DIST, 0.0, 1.0));

        // Assign tess levels (float values required)
        gl_TessLevelOuter[0] = tes0; // Left
        gl_TessLevelOuter[1] = tes1; // Bottom
        gl_TessLevelOuter[2] = tes2; // Right
        gl_TessLevelOuter[3] = tes3; // Top

        gl_TessLevelInner[0] = max(tes1, tes3); // Horizontal inner
        gl_TessLevelInner[1] = max(tes0, tes2); // Vertical inner
    }
}
