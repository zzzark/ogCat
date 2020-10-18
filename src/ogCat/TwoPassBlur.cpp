#include "TwoPassBlur.h"

static const char* effect_v_blur_fs =
"#version 450 core                                                           \n"
"in vec2 v_uv;                                                               \n"
"uniform sampler2D u_tex;                                                    \n"
"uniform float u_height;                                                     \n"
"out vec3 cl;                                                                \n"
"const float kernel[5] =                                                     \n"
"    float[] (0.2, 0.1, 0.1, 0.1, 0.1);                                      \n"
//"    float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);           \n"
"void main()                                                                 \n"
"{                                                                           \n"
"    cl = texture(u_tex, v_uv).xyz * kernel[0];                              \n"
"    for(int i = 1; i <= 4; i++) {                                           \n"
"        cl += texture(u_tex, v_uv + vec2(0.0, i/u_height)).xyz * kernel[i]; \n"
"        cl += texture(u_tex, v_uv - vec2(0.0, i/u_height)).xyz * kernel[i]; \n"
"    }                                                                       \n"
"}                                                                           \n"
;
static const char* effect_h_blur_fs =
"#version 450 core                                                           \n"
"in vec2 v_uv;                                                               \n"
"uniform sampler2D u_tex;                                                    \n"
"uniform float u_width;                                                      \n"
"out vec3 cl;                                                                \n"
"const float kernel[5] =                                                     \n"
"    float[] (0.2, 0.1, 0.1, 0.1, 0.1);                                      \n"
//"    float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);           \n"
"void main()                                                                 \n"
"{                                                                           \n"
"    cl = texture(u_tex, v_uv).xyz * kernel[0];                              \n"
"    for(int i = 1; i <= 4; i++) {                                           \n"
"        cl += texture(u_tex, v_uv + vec2(i/u_width, 0.0)).xyz * kernel[i];  \n"
"        cl += texture(u_tex, v_uv - vec2(i/u_width, 0.0)).xyz * kernel[i];  \n"
"    }                                                                       \n"
"}                                                                           \n"
;
void cat::effectBlur::create(unsigned int w, unsigned int h)
{
	_eff_v.create(effect_v_blur_fs);
	_eff_h.create(effect_h_blur_fs);
	shader::uniform u_w = _eff_h.shader()["u_width"];
	shader::uniform u_h = _eff_v.shader()["u_height"];
	u_w = (float)w;
	u_h = (float)h;
	_tmpBuffer.createEX(w, h, 1, 3, 16U, false, false);
}

void cat::effectBlur::applyEffect(const frameBuffer* destBuffer, const texture2D& originalTex)
{
	{
		originalTex.active();
		_tmpBuffer.bindForBoth();
		_eff_v.renderPURE();
		_tmpBuffer.unbindForBoth();
	}
	{
		_tmpBuffer[0].active();
		if (destBuffer) destBuffer->bindForBoth();
		_eff_h.renderPURE();
		if (destBuffer) destBuffer->unbindForBoth();
	}

}
