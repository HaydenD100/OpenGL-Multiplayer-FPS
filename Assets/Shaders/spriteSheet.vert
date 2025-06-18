#version 460 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

uniform int u_rowCount;
uniform int u_columnCount;
uniform int u_billboard;
uniform vec4 u_position;
uniform vec4 u_rotation;
uniform vec4 u_scale;
uniform int u_frameIndex;
uniform int u_frameNextIndex;


uniform mat4 P;
uniform mat4 V;

uniform mat4 inverseV;

out vec2 TexCoord;
out vec2 TexCoordNext;

mat4 ToMat4(vec3 position, vec3 rotation, vec3 scale) {
    // Translation matrix
    mat4 translationMatrix = mat4(1.0);
    translationMatrix[3] = vec4(position, 1.0);

    // Rotation matrices (XYZ Euler Order)
    float cosX = cos(rotation.x), sinX = sin(rotation.x);
    float cosY = cos(rotation.y), sinY = sin(rotation.y);
    float cosZ = cos(rotation.z), sinZ = sin(rotation.z);

    mat4 rotX = mat4(
        1,  0,    0,   0,
        0,  cosX, -sinX, 0,
        0,  sinX, cosX, 0,
        0,  0,    0,   1
    );

    mat4 rotY = mat4(
        cosY,  0, sinY,  0,
        0,     1, 0,     0,
        -sinY, 0, cosY,  0,
        0,     0, 0,     1
    );

    mat4 rotZ = mat4(
        cosZ, -sinZ, 0, 0,
        sinZ, cosZ,  0, 0,
        0,    0,     1, 0,
        0,    0,     0, 1
    );

    // Combined rotation (Z * Y * X order)
    mat4 rotationMatrix = rotZ * rotY * rotX;

    // Scale matrix
    mat4 scaleMatrix = mat4(1.0);
    scaleMatrix[0][0] = scale.x;
    scaleMatrix[1][1] = scale.y;
    scaleMatrix[2][2] = scale.z;

    // Final transformation matrix
    return translationMatrix * rotationMatrix * scaleMatrix;
}

void main() {

    float frameWidth = 1.0 / u_columnCount;
    float frameHeight = 1.0 / u_rowCount;
    
    int frameX = u_frameIndex % u_columnCount;
    int frameY = (u_frameIndex - (u_frameIndex % u_columnCount)) / u_columnCount;
    vec2 frameOffset = vec2(frameX * frameWidth, frameY * frameHeight);
    TexCoord = frameOffset + vertexUV * vec2(frameWidth, frameHeight);

    int frameNextX = u_frameNextIndex % u_columnCount;
    int frameNextY = (u_frameNextIndex - (u_frameNextIndex % u_columnCount)) / u_columnCount;
    vec2 frameNextOffset = vec2(frameNextX * frameWidth, frameNextY * frameHeight);
    TexCoordNext = frameNextOffset + vertexUV * vec2(frameWidth, frameHeight);
    
    bool billboard = true;
    int viewportIndex = gl_BaseInstance;
	mat4 projectionView = P * V;
	mat4 inverseView = inverseV;
    
    vec3 position = u_position.xyz;
    vec3 rotation = u_rotation.xyz;
    vec3 scale = u_scale.xyz;

    mat4 modelMatrix = ToMat4(position, rotation, scale);

    //if (u_billboard == 1) {
        vec3 worldPosition = (modelMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz;

        // Extract scale from model matrix
        vec3 scale2 = vec3(
            length(modelMatrix[0].xyz),
            length(modelMatrix[1].xyz),
            length(modelMatrix[2].xyz)
        );

        mat4 localMatrix = ToMat4(vec3(0,0,0), rotation, scale);

        // Camera basis vectors
        vec3 cameraRight = normalize(inverseView[0].xyz);
        vec3 cameraUp = normalize(inverseView[1].xyz);
        vec3 cameraForward = normalize(-inverseView[2].xyz);

        // Construct the billboard matrix
        mat4 billboardMatrix = mat4(1.0);
        billboardMatrix[0] = vec4(cameraRight, 0.0);
        billboardMatrix[1] = vec4(cameraUp, 0.0);
        billboardMatrix[2] = vec4(cameraForward, 0.0);
        billboardMatrix[3] = vec4(worldPosition, 1.0);

        modelMatrix = billboardMatrix * localMatrix;
  //  }


    gl_Position = projectionView * modelMatrix * vec4(vertexPosition_modelspace, 1.0);
}