#include "FXAA.h"

static const char* effect_fxaa_fs =
"#version 450 core                                                                                   \n"
"in vec2 v_uv;                                                                                       \n"
"uniform sampler2D u_tex;                                                                            \n"
"uniform float u_w;                                                                                  \n"
"uniform float u_h;                                                                                  \n"
"const vec3 luma = vec3(0.299, 0.587, 0.114);                                                        \n"
"const vec2 frameBufSize = vec2(u_w, u_h);                                                           \n"
"void main( void ) {                                                                                 \n"
"    vec2 texCoords = v_uv;                                                                          \n"
"    float FXAA_SPAN_MAX = 8.0;                                                                      \n"
"    float FXAA_REDUCE_MUL = 1.0/8.0;                                                                \n"
"    float FXAA_REDUCE_MIN = 1.0/128.0;                                                              \n"
"                                                                                                    \n"
"    vec3 rgbNW=texture2D(u_tex,texCoords+(vec2(-1.0,-1.0)/frameBufSize)).xyz;                       \n"
"    vec3 rgbNE=texture2D(u_tex,texCoords+(vec2(1.0,-1.0)/frameBufSize)).xyz;                        \n"
"    vec3 rgbSW=texture2D(u_tex,texCoords+(vec2(-1.0,1.0)/frameBufSize)).xyz;                        \n"
"    vec3 rgbSE=texture2D(u_tex,texCoords+(vec2(1.0,1.0)/frameBufSize)).xyz;                         \n"
"    vec3 rgbM=texture2D(u_tex,texCoords).xyz;                                                       \n"
"                                                                                                    \n"
"    float lumaNW = dot(rgbNW, luma);                                                                \n"
"    float lumaNE = dot(rgbNE, luma);                                                                \n"
"    float lumaSW = dot(rgbSW, luma);                                                                \n"
"    float lumaSE = dot(rgbSE, luma);                                                                \n"
"    float lumaM  = dot(rgbM,  luma);                                                                \n"
"                                                                                                    \n"
"    vec2 dir;                                                                                       \n"
"    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));                                               \n"
"    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));                                               \n"
"                                                                                                    \n"
"    float dirReduce = max(                                                                          \n"
"        (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),                             \n"
"        FXAA_REDUCE_MIN);                                                                           \n"
"    float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);                                \n"
"                                                                                                    \n"
"    dir = min(vec2( FXAA_SPAN_MAX,  FXAA_SPAN_MAX),                                                 \n"
"          max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),                                                 \n"
"          dir * rcpDirMin)) / frameBufSize;                                                         \n"
"                                                                                                    \n"
"    vec3 rgb = (1.0/4.0) * (                                                                        \n"
"        texture2D(u_tex, texCoords.xy + dir * (1.0/3.0 - 0.5)).xyz +                                \n"
"        texture2D(u_tex, texCoords.xy + dir * (2.0/3.0 - 0.5)).xyz +                                \n"
"        texture2D(u_tex, texCoords.xy + dir * (0.0/3.0 - 0.5)).xyz +                                \n"
"        texture2D(u_tex, texCoords.xy + dir * (3.0/3.0 - 0.5)).xyz                                  \n"
"    );                                                                                              \n"
"    gl_FragColor.xyz=rgb;                                                                           \n"
"}                                                                                                   \n"
;
cat::effectFXAA::effectFXAA()
{
	this->create();
	_lastTex = nullptr;
}

cat::effectFXAA::effectFXAA(bool autoCreate)
{
	if(autoCreate)
		this->create();
	_lastTex = nullptr;
}

void cat::effectFXAA::applyEffect(const texture2D& originalTex)
{
	if (_lastTex != &originalTex) {
		_lastTex = &originalTex;
		shader::uniform u_w = frameEffect::shader()["u_w"];
		shader::uniform u_h = frameEffect::shader()["u_h"];
		u_w = (float)originalTex.getWidth();
		u_h = (float)originalTex.getHeight();
	}
	originalTex.active();
	frameEffect::renderPURE();
}

void cat::effectFXAA::create()
{
	frameEffect::create(effect_fxaa_fs);
}