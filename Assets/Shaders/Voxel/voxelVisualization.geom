#version 460 core

layout(points) in;               // Input type: points from the vertex shader
layout(triangle_strip, max_vertices = 24) out; // Output type: triangle strip (24 vertices for 6 faces of a cube)

in vec3 gridCoord[];             // Input from vertex shader
in vec4 voxelColor[];
in vec3 worldPos[];// Input from vertex shader

in mat4 projView[];      // Projection matrix

out vec4 fragColor;              // Output color to fragment shader


// Define the 8 vertices of a cube centered at (0, 0, 0)
const float halfSize = 0.2499; 

void main()
{
    // Define the 8 vertices of a cube relative to its center
    vec3 vertices[8] = vec3[](
        vec3(-halfSize, -halfSize, -halfSize),
        vec3( halfSize, -halfSize, -halfSize),
        vec3(-halfSize,  halfSize, -halfSize),
        vec3( halfSize,  halfSize, -halfSize),
        vec3(-halfSize, -halfSize,  halfSize),
        vec3( halfSize, -halfSize,  halfSize),
        vec3(-halfSize,  halfSize,  halfSize),
        vec3( halfSize,  halfSize,  halfSize)
    );

    // Define the 6 faces (2 triangles per face)
    int indices[24] = int[](
        0, 1, 2, 3,  // Bottom face (quad)
        4, 5, 6, 7,  // Top face (quad)
        0, 2, 4, 6,  // Left face (quad)
        1, 3, 5, 7,  // Right face (quad)
        0, 1, 4, 5,  // Front face (quad)
        2, 3, 6, 7   // Back face (quad)
    );

    // Emit vertices for the cube
    for (int i = 0; i < 24; ++i)
    {
        vec3 position = worldPos[0] + vertices[indices[i]]; // Offset vertex by grid coordinates
        gl_Position = projView[0] * vec4(position, 1.0);
        fragColor = voxelColor[0]; // Pass color to fragment shader
        EmitVertex();
    }
    EndPrimitive();
}