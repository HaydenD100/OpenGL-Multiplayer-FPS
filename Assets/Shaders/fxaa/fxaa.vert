#version 430 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;


uniform float viewportWidth;
uniform float viewportHeight;

out vec2 TexCoords;
out vec2 texCoordOffset;
out vec2 offsetTL;
out vec2 offsetTR;
out vec2 offsetBL;
out vec2 offsetBR;




void texcoords(vec2 fragCoord, vec2 resolution, out vec2 v_rgbNW, out vec2 v_rgbNE, out vec2 v_rgbSW, out vec2 v_rgbSE, out vec2 v_rgbM) {
	vec2 inverseVP = 1.0 / resolution.xy;
	v_rgbNW = (fragCoord + vec2(-1.0, -1.0)) * inverseVP;
	v_rgbNE = (fragCoord + vec2(1.0, -1.0)) * inverseVP;
	v_rgbSW = (fragCoord + vec2(-1.0, 1.0)) * inverseVP;
	v_rgbSE = (fragCoord + vec2(1.0, 1.0)) * inverseVP;
	v_rgbM = vec2(fragCoord * inverseVP);
}




void main() {  

    TexCoords = (vertexPosition_modelspace.xy+vec2(1,1))/2.0;

	// Compute Fxaa texture offsets here, rather than fragment shader
	texCoordOffset = 1.0f / vec2(float(viewportWidth), float(viewportHeight));
	//texCoordOffset *= 0.75;
	offsetTL = TexCoords + vec2(-1.0, -1.0) * texCoordOffset;
	offsetTR = TexCoords + (vec2(1.0, -1.0) * texCoordOffset);
	offsetBL = TexCoords + vec2(-1.0,  1.0) * texCoordOffset;
	offsetBR = TexCoords + vec2( 1.0,  1.0) * texCoordOffset;

    gl_Position = vec4(vertexPosition_modelspace, 1.0);
}