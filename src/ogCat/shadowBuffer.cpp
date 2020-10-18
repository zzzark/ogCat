#include "shadowBuffer.h"
#include "renderer.h"

static const char* v_fs =
"#version 450 core                                                               \n"
"in vec2 v_uv;                                                                   \n"
"uniform sampler2D u_tex;                                                        \n"
"uniform sampler2D u_reg;                                                        \n"
"uniform sampler2D u_col;                                                        \n"
"uniform float u_height;                                                         \n"
"out vec3 cl;                                                                    \n"
"const float kernel[5] =                                                         \n"
"    float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);               \n"
"const vec3 luma = vec3(0.299, 0.587, 0.114);                                    \n"
"void main()                                                                     \n"
"{                                                                               \n"
"    vec3 base = texture(u_tex, v_uv).xyz;                                       \n"
"    float ML = dot(texture(u_col, v_uv).xyz, luma);                             \n"
"    float MR = texture(u_reg, v_uv).x;                                          \n"
"    cl = base * kernel[0];                                                      \n"
"    for(int i = 1; i <= 4; i++) {                                               \n"
"        float NL = dot(texture(u_col, v_uv + vec2(0.0, i/u_height)).xyz, luma); \n"
"        float NR = texture(u_reg, v_uv + vec2(0.0, i/u_height)).x;              \n"
"        vec3 selCol = (abs(ML - NL) <= 0.125 && abs(MR - NR) <= 0.1) ?          \n"
"                       texture(u_tex, v_uv + vec2(0.0, i/u_height)).xyz : base; \n"
"        cl += selCol * kernel[i];                                               \n"
"    }                                                                           \n"
"    for(int i = 1; i <= 4; i++) {                                               \n"
"        float NL = dot(texture(u_col, v_uv - vec2(0.0, i/u_height)).xyz, luma); \n"
"        float NR = texture(u_reg, v_uv - vec2(0.0, i/u_height)).x;              \n"
"        vec3 selCol = (abs(ML - NL) <= 0.125 && abs(MR - NR) <= 0.1) ?          \n"
"                       texture(u_tex, v_uv - vec2(0.0, i/u_height)).xyz : base; \n"
"        cl += selCol * kernel[i];                                               \n"
"    }                                                                           \n"
"}                                                                               \n"
;
static const char* h_fs =
"#version 450 core                                                               \n"
"in vec2 v_uv;                                                                   \n"
"uniform sampler2D u_tex;                                                        \n"
"uniform sampler2D u_reg;                                                        \n"
"uniform sampler2D u_col;                                                        \n"
"uniform float u_width;                                                          \n"
"out vec3 cl;                                                                    \n"
"const float kernel[5] =                                                         \n"
"    float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);               \n"
"const vec3 luma = vec3(0.299, 0.587, 0.114);                                    \n"
"void main()                                                                     \n"
"{                                                                               \n"
"    vec3 base = texture(u_tex, v_uv).xyz;                                       \n"
"    float ML = dot(texture(u_col, v_uv).xyz, luma);                             \n"
"    float MR = texture(u_reg, v_uv).x;                                          \n"
"    cl = base * kernel[0];                                                      \n"
"    for(int i = 1; i <= 4; i++) {                                               \n"
"        float NL = dot(texture(u_col, v_uv + vec2(i/u_width, 0.0)).xyz, luma);  \n"
"        float NR = texture(u_reg, v_uv + vec2(i/u_width, 0.0)).x;               \n"
"        vec3 selCol = (abs(ML - NL) <= 0.125 && abs(MR - NR) <= 0.1) ?          \n"
"                       texture(u_tex, v_uv + vec2(i/u_width, 0.0)).xyz : base;  \n"
"        cl += selCol * kernel[i];                                               \n"
"    }                                                                           \n"
"    for(int i = 1; i <= 4; i++) {                                               \n"
"        float NL = dot(texture(u_col, v_uv - vec2(i/u_width, 0.0)).xyz, luma);  \n"
"        float NR = texture(u_reg, v_uv - vec2(i/u_width, 0.0)).x;               \n"
"        vec3 selCol = (abs(ML - NL) <= 0.125 && abs(MR - NR) <= 0.1) ?          \n"
"                       texture(u_tex, v_uv - vec2(i/u_width, 0.0)).xyz : base;  \n"
"        cl += selCol * kernel[i];                                               \n"
"    }                                                                           \n"
"}                                                                               \n"
;

cat::shadowBuffer::shadowBuffer()
{
	_next_bind = 0;
}

void cat::shadowBuffer::setScaleFactor(float factor)
{
	shader::uniform w = _eff_h.shader()["u_width"];
	shader::uniform h = _eff_v.shader()["u_height"];
	w = factor * (float)_sha[0].getWidth();
	h = factor * (float)_sha[0].getHeight();
}

void cat::shadowBuffer::create(unsigned int width, unsigned int height, unsigned int bit, float scaleFactor)
{
	_next_bind = 0;
	_sha.createEX(width, height, 2, 3U, bit, false, false);
	_sha.getTex(0).setfilter(TEXTURE_FILTER::TF_LINEAR);
	_sha.getTex(1).setfilter(TEXTURE_FILTER::TF_LINEAR);
	_sha.bindForBoth();
	_sha.switchBuffer(0);
	_sha.unbindForBoth();

	_eff_v.create(v_fs);
	_eff_h.create(h_fs);
	shader::uniform w = _eff_h.shader()["u_width"];
	shader::uniform h = _eff_v.shader()["u_height"];
	w = scaleFactor * (float)width;
	h = scaleFactor * (float)height;
	{
		shader::uniform tex = _eff_h.shader()["u_tex"];
		shader::uniform reg = _eff_h.shader()["u_reg"];
		shader::uniform col = _eff_h.shader()["u_col"];
		tex = (int)0;
		reg = (int)1;
		col = (int)2;
	}
	{
		shader::uniform tex = _eff_v.shader()["u_tex"];
		shader::uniform reg = _eff_v.shader()["u_reg"];
		shader::uniform col = _eff_v.shader()["u_col"];
		tex = (int)0;
		reg = (int)1;
		col = (int)2;
	}
}

void cat::shadowBuffer::applyEffect(const cat::gbuffer& gbuf) const
{
	unsigned int next = _next_bind ? 1 : 0;
	unsigned int prev = _next_bind ? 0 : 1;

	_sha.bindForBoth();

	_sha.switchBuffer(next);
	_sha[prev].active(0);
	gbuf.reg_tex().active(1);
	gbuf.col_tex().active(2);
	_eff_v.renderPURE();

	_sha.switchBuffer(prev);
	_sha[next].active(0);
	gbuf.reg_tex().active(1);
	gbuf.col_tex().active(2);
	_eff_h.renderPURE();

	_sha.unbindForBoth();
}

void cat::shadowBuffer::begin()
{
	_sha.bindForBoth();
	_sha.switchBuffers(2);
	renderer::ClearAll();
}

void cat::shadowBuffer::end()
{
	_sha.unbindForBoth();
}

void cat::shadowBuffer::begin_shadow()
{
	_sha.switchBuffer(_next_bind);
}

void cat::shadowBuffer::end_shadow()
{
	_next_bind = _next_bind ? 0 : 1;
}

const cat::texture2D& cat::shadowBuffer::getTex() const
{
	unsigned int prev = _next_bind ? 0 : 1;
	return _sha.getTex(prev);
}
