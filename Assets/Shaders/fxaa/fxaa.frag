#version 430 core

out vec4 FragColor;

// Input texture and texture coordinates
layout (binding = 0) uniform sampler2D inputTexture;

in vec2 TexCoords;
in vec2 offsetTL;
in vec2 offsetTR;
in vec2 offsetBL;
in vec2 offsetBR;
in vec2 texCoordOffset;

//TokyoSplif code

// FXAA implementation
vec3 Fxaa(sampler2D tex) {
    // Constants for FXAA
    float fxaaSpanMax = 8.0;
    float fxaaReduceMin = 1.0 / 128.0;
    float fxaaReduceMul = 1.0 / 4.0;

    // Luma weights for RGB channels
    vec3 lumaWeights = vec3(0.299, 0.587, 0.114);

    // Sample luma values from the texture
    float lumaTL = dot(lumaWeights, texture(tex, offsetTL).rgb);
    float lumaTR = dot(lumaWeights, texture(tex, offsetTR).rgb);
    float lumaBL = dot(lumaWeights, texture(tex, offsetBL).rgb);
    float lumaBR = dot(lumaWeights, texture(tex, offsetBR).rgb);
    float lumaM = dot(lumaWeights, texture(tex, TexCoords).rgb);

    // Calculate the direction of the edge
    vec2 dir;
    dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
    dir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));

    // Reduce the direction's influence
    float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (fxaaReduceMul * 0.25), fxaaReduceMin);
    float inverseDirAdjustment = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = clamp(dir * inverseDirAdjustment, vec2(-fxaaSpanMax), vec2(fxaaSpanMax)) * texCoordOffset;

    // Sample along the calculated direction
    vec3 result1 = 0.5 * (
        texture(tex, TexCoords + dir * (1.0 / 3.0 - 0.5)).rgb +
        texture(tex, TexCoords + dir * (2.0 / 3.0 - 0.5)).rgb
    );

    vec3 result2 = 0.5 * result1 + 0.25 * (
        texture(tex, TexCoords + dir * (0.0 / 3.0 - 0.5)).rgb +
        texture(tex, TexCoords + dir * (3.0 / 3.0 - 0.5)).rgb
    );

    // Compute min and max luma for clamping
    float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
    float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));

    // Select the final result
    float lumaResult2 = dot(lumaWeights, result2);
    if (lumaResult2 < lumaMin || lumaResult2 > lumaMax)
        return result1;
    else
        return result2;
}

void main() {
    vec3 result = Fxaa(inputTexture);
    //result = texture(inputTexture, TexCoords).xyz;
    // Output the final color
    FragColor = vec4(result, 1.0);
}