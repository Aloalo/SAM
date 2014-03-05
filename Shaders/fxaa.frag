#version 420 core

layout (binding = 0) uniform sampler2D renderedTexture;
layout (location = 0) out vec4 fOut;

#define FXAA_REDUCE_MIN (1.0 / 128.0)
#define FXAA_REDUCE_MUL (1.0 / 8.0)
#define FXAA_SPAN_MAX 8.0

noperspective in vec2 UV;

void main()
{
	vec2 texSize = vec2(textureSize(renderedTexture, 0));
	vec2 texelSize = vec2(1.0 / texSize);

    vec3 rgbNW = texture(renderedTexture,UV + vec2(-1.0, -1.0) * texelSize).xyz;
    vec3 rgbNE = texture(renderedTexture,UV + vec2( 1.0, -1.0) * texelSize).xyz;
    vec3 rgbSW = texture(renderedTexture,UV + vec2(-1.0,  1.0) * texelSize).xyz;
    vec3 rgbSE = texture(renderedTexture,UV + vec2( 1.0,  1.0) * texelSize).xyz;
    vec3 rgbM  = texture(renderedTexture,UV).xyz;
    vec3 luma  = vec3(0.299, 0.587, 0.114);

    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM =  dot(rgbM, luma);
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * texelSize;

	/*if(dot(dir, dir) > 0.00005)
		fOut = vec4(1.0, 1.0, 1.0, 1.0);
	else
		fOut = vec4(0.0, 0.0, 0.0, 1.0);
	return;*/

    vec3 rgbA = 0.5 * (
        texture(renderedTexture, UV + dir * (1.0 / 3.0 - 0.5)).xyz+
		texture(renderedTexture, UV + dir * (2.0 / 3.0 - 0.5)).xyz);

    vec3 rgbB = rgbA * 0.5 + 0.25 * (
        texture(renderedTexture, UV + dir * -0.5).xyz+
		texture(renderedTexture, UV + dir * 0.5).xyz);

    float lumaB = dot(rgbB, luma);

    if ((lumaB < lumaMin) || (lumaB > lumaMax)) {
        fOut = vec4(rgbA, 1.0);
    } else {
        fOut = vec4(rgbB, 1.0);
    }
}
