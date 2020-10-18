#include "postprc.h"
#include "common.h"

static const char* __bloom_depart =
"#version 450 core                                      \n"
"in vec2 v_uv;                                          \n"
"uniform sampler2D u_col;                               \n"
"uniform sampler2D u_sha;                               \n"
"uniform sampler2D u_emm;                               \n"
"layout(location = 0) out vec3 color;                   \n"
"layout(location = 1) out vec3 bloom;                   \n"
"uniform vec3 u_threshold = vec3(1.0f);                 \n"
"void main()                                            \n"
"{                                                      \n"
"    vec3 col = texture(u_col, v_uv).xyz;               \n"
"    vec3 sha = texture(u_sha, v_uv).xyz * col          \n"
"             + texture(u_emm, v_uv).xyz;               \n"
"    color = sha;                                       \n"
"    bloom = max(sha - u_threshold, vec3(0.0f));        \n"
"}                                                      \n"
"                                                       \n"
;

static const char* __bloom_blur_V = 
"#version 450 core                                                           \n"
"in vec2 v_uv;                                                               \n"
"uniform sampler2D u_tex;                                                    \n"
"uniform float u_height;                                                     \n"
"out vec3 cl;                                                                \n"
"const float kernel[5] =                                                     \n"
"    float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);           \n"
"void main()                                                                 \n"
"{                                                                           \n"
"    cl = texture(u_tex, v_uv).xyz * kernel[0];                              \n"
"    for(int i = 1; i <= 4; i++) {                                           \n"
"        cl += texture(u_tex, v_uv + vec2(0.0, i/u_height)).xyz * kernel[i]; \n"
"        cl += texture(u_tex, v_uv - vec2(0.0, i/u_height)).xyz * kernel[i]; \n"
"    }                                                                       \n"
"}                                                                           \n"
;

static const char* __bloom_blur_H =
"#version 450 core                                                           \n"
"in vec2 v_uv;                                                               \n"
"uniform sampler2D u_tex;                                                    \n"
"uniform float u_width;                                                      \n"
"out vec3 cl;                                                                \n"
"const float kernel[5] =                                                     \n"
"    float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);           \n"
"void main()                                                                 \n"
"{                                                                           \n"
"    cl = texture(u_tex, v_uv).xyz * kernel[0];                              \n"
"    for(int i = 1; i <= 4; i++) {                                           \n"
"        cl += texture(u_tex, v_uv + vec2(i/u_width, 0.0)).xyz * kernel[i];  \n"
"        cl += texture(u_tex, v_uv - vec2(i/u_width, 0.0)).xyz * kernel[i];  \n"
"    }                                                                       \n"
"}                                                                           \n"
;

static const char* __hdr_combine =
"#version 450 core                           \n"
"in vec2 v_uv;                               \n"
"uniform sampler2D u_org;                    \n"
"uniform sampler2D u_blm;                    \n"
"uniform float u_factor = 1.22;              \n"
"out vec3 cl;                                \n"
"void main()                                 \n"
"{                                           \n"
"    vec3 sha = texture(u_org, v_uv).xyz     \n"
"  + u_factor * texture(u_blm, v_uv).xyz;    \n"
"                                            \n"
"    sha = pow(sha, vec3(2.2));              \n"
"    sha = sha / (sha + vec3(1.0));          \n"
"    cl  = pow(sha, vec3(1.0 / 2.2));        \n"
"}                                           \n"
;

static const char* __fxaa = 
"#version 450 core                                                                 \n"
"in vec2 v_uv;                                                                     \n"
"uniform sampler2D u_tex;                                                          \n"
"uniform vec2 frameBufSize;                                                        \n"
"const vec3 luma = vec3(0.299, 0.587, 0.114);                                      \n"
"out vec3 color;                                                                   \n"
"void main()                                                                       \n"
"{                                                                                 \n"
"    const vec2 texCoords = v_uv;                                                  \n"
"    const float FXAA_SPAN_MAX = 8.0;                                              \n"
"    const float FXAA_REDUCE_MUL = 1.0/8.0;                                        \n"
"    const float FXAA_REDUCE_MIN = 1.0/64.0;                                       \n"
"                                                                                  \n"
"    vec3 rgbNW=texture(u_tex,texCoords+(vec2(-1.0,-1.0)/frameBufSize)).xyz;       \n"
"    vec3 rgbNE=texture(u_tex,texCoords+(vec2( 1.0,-1.0)/frameBufSize)).xyz;       \n"
"    vec3 rgbSW=texture(u_tex,texCoords+(vec2(-1.0, 1.0)/frameBufSize)).xyz;       \n"
"    vec3 rgbSE=texture(u_tex,texCoords+(vec2( 1.0, 1.0)/frameBufSize)).xyz;       \n"
"    vec3 rgbM =texture(u_tex,texCoords).xyz;                                      \n"
"                                                                                  \n"
"    float lumaNW = dot(rgbNW, luma);                                              \n"
"    float lumaNE = dot(rgbNE, luma);                                              \n"
"    float lumaSW = dot(rgbSW, luma);                                              \n"
"    float lumaSE = dot(rgbSE, luma);                                              \n"
"    float lumaM  = dot(rgbM,  luma);                                              \n"
"                                                                                  \n"
"    vec2 dir;                                                                     \n"
"    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));                             \n"
"    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));                             \n"
"                                                                                  \n"
"    float dirReduce = max(                                                        \n"
"        (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),           \n"
"        FXAA_REDUCE_MIN);                                                         \n"
"    float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);              \n"
"                                                                                  \n"
"    dir = min(vec2( FXAA_SPAN_MAX,  FXAA_SPAN_MAX),                               \n"
"          max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),                               \n"
"          dir * rcpDirMin)) / frameBufSize;                                       \n"
"                                                                                  \n"
"    color = (1.0/4.0) * (                                                         \n"
"        texture2D(u_tex, texCoords + dir * (1.0/3.0 - 0.5)).xyz +                 \n"
"        texture2D(u_tex, texCoords + dir * (2.0/3.0 - 0.5)).xyz +                 \n"
"        texture2D(u_tex, texCoords + dir * (0.0/3.0 - 0.5)).xyz +                 \n"
"        texture2D(u_tex, texCoords + dir * (3.0/3.0 - 0.5)).xyz                   \n"
"    );                                                                            \n"
"}                                                                                 \n"
;
//static const char* _motion_blur =
//"#version 450 core                                                  \n"
//"in vec2 v_uv;                                                      \n"
//"uniform sampler2D u_fnl;                                           \n"
//"uniform sampler2D u_pos;                                           \n"
//"out vec3 color;                                                    \n"
//"uniform mat4 u_last_mvp;                                           \n"
//"const float dec[4] =                                               \n"
//"    float[] (0.25, 0.25, 0.25, 0.25);                              \n"
//"void main()                                                        \n"
//"{                                                                  \n"
//"    vec3 total = vec3(0.0f);                                       \n"
//"    vec4 frag = u_last_mvp * vec4(texture(u_pos, v_uv).xyz, 1.0f); \n"
//"    vec2 dir = (frag.xy / frag.w) * 0.5f + vec2(0.5f) - v_uv;      \n"
//"    total += texture(u_fnl, v_uv + (0.0 / 3.0) * dir).xyz * dec[0];\n"
//"    total += texture(u_fnl, v_uv + (1.0 / 3.0) * dir).xyz * dec[1];\n"
//"    total += texture(u_fnl, v_uv + (2.0 / 3.0) * dir).xyz * dec[2];\n"
//"    total += texture(u_fnl, v_uv + (3.0 / 3.0) * dir).xyz * dec[3];\n"
//"    color = total;                                                 \n"
//"                                                                   \n"
//"}                                                                  \n"
//;

//static const char* __depth =
//"#version 450 core                                                  \n"
//"in vec2 v_uv;                                                      \n"
//"uniform sampler2D u_rel;                                           \n"
//"uniform sampler2D u_blr;                                           \n"
//"uniform sampler2D u_pos;                                           \n"
//"uniform vec3 u_dir;                                                \n"
//"uniform vec3 u_eye;                                                \n"
//"uniform float u_oneDivLen;                                         \n"
//"out vec3 color;                                                    \n"
//"void main()                                                        \n"
//"{                                                                  \n"
//"    vec3 dir = texture(u_pos, v_uv).xyz - u_eye;                   \n"
//"    float factor = max(1.0f,                                       \n"
//"                       abs(1.0f - dot(dir, u_dir) * u_oneDivLen)); \n"
//"    color = mix(texture(u_rel, v_uv).xyz,                          \n"
//"                texture(u_blr, v_uv).xyz,                          \n"
//"                factor);                                           \n"
//"                                                                   \n"
//"}                                                                  \n"
//;

void cat::postprc::create(gbuffer& gbf)
{
	/* --- effects --- */
	_eff_bloom_depart.create(__bloom_depart);
	shader::uniform u_col = _eff_bloom_depart.shader()["u_col"];
	shader::uniform u_sha = _eff_bloom_depart.shader()["u_sha"];
	shader::uniform u_emm = _eff_bloom_depart.shader()["u_emm"];
	u_col = (int)0;
	u_sha = (int)1;
	u_emm = (int)2;

	_eff_bloom_blur_V.create(__bloom_blur_V);
	shader::uniform u_hei = _eff_bloom_blur_V.shader()["u_height"];
	shader::uniform u_txv = _eff_bloom_blur_V.shader()["u_tex"];
	u_hei = 1.0f * (float)gbf.getHeight();
	u_txv = (int)0;

	_eff_bloom_blur_H.create(__bloom_blur_H);
	shader::uniform u_wid = _eff_bloom_blur_H.shader()["u_width"];
	shader::uniform u_txh = _eff_bloom_blur_H.shader()["u_tex"];
	u_wid = 1.0f * (float)gbf.getWidth();
	u_txh = (int)0;

	_eff_hdr_combine.create(__hdr_combine);
	shader::uniform u_org = _eff_hdr_combine.shader()["u_org"];
	shader::uniform u_blm = _eff_hdr_combine.shader()["u_blm"];
	u_org = (int)0;
	u_blm = (int)1;

	_eff_fxaa.create(__fxaa);
	shader::uniform u_tex = _eff_fxaa.shader()["u_tex"];
	shader::uniform u_wh  = _eff_fxaa.shader()["frameBufSize"];
	u_wh = glm::vec2((float)gbf.getWidth(), (float)gbf.getHeight());
	u_tex = (int)0;

	//_eff_depth_blur.create(__depth);
	//shader::uniform u_rel = _eff_depth_blur.shader()["u_rel"];
	//shader::uniform u_blr = _eff_depth_blur.shader()["u_blr"];
	//shader::uniform u_pos = _eff_depth_blur.shader()["u_pos"];
	//u_rel = (int)0;
	//u_blr = (int)1;
	//u_pos = (int)2;
	//_loc_depth_eye = _eff_depth_blur.shader().getlocation("u_eye");
	//_loc_depth_dir = _eff_depth_blur.shader().getlocation("u_dir");
	//_loc_depth_len = _eff_depth_blur.shader().getlocation("u_oneDivLen");

	//_eff_motionBlur.create(_motion_blur);
	//shader::uniform u_fnl = _eff_motionBlur.shader()["u_fnl"];
	//shader::uniform u_pos = _eff_motionBlur.shader()["u_pos"];
	//_loc_last_mvp = _eff_motionBlur.shader().getlocation("u_last_mvp");
	//u_fnl = (int)0;
	//u_pos = (int)1;

	/* --- buffers --- */
	_bloom.createEX(gbf.getWidth(), gbf.getHeight(), 2, 3U, 16U, false, false);
	_tmpbuf.createEX(gbf.getWidth(), gbf.getHeight(), 1, 3U, 16U, false, false);
	//_final[0].create(gbf.getWidth(), gbf.getHeight(), 1, false, false);
	//_final[1].create(gbf.getWidth(), gbf.getHeight(), 1, false, false);
	//_final[0][0].setwrap(TEXTURE_WRAP::TW_CLAMP_TO_EDGE);
	//_final[1][0].setwrap(TEXTURE_WRAP::TW_CLAMP_TO_EDGE);
	
	/* --- members --- */
	_last_mvp = glm::mat4(1.0f);
}

void cat::postprc::process(gbuffer& gbf, shadowBuffer& sbf, camera& org)
{
	renderer::SetViewPort(0, 0, gbf.getWidth(), gbf.getHeight());
	glDisable(GL_DEPTH_TEST);

	/* ---- depart ---- */
	_bloom.bindForBoth();
	_bloom.switchBuffers(2);
	renderer::ClearColor();
	gbf.col_tex().active(0);
	sbf.getTex().active(1);
	gbf.emm_tex().active(2);
	_eff_bloom_depart.renderPURE();
	_bloom.unbindForBoth();

	/* ---- bloom blur ---- */
	_tmpbuf.bindForBoth();
	renderer::ClearColor();
	_bloom[1].active(0);
	_eff_bloom_blur_V.renderPURE();
	_tmpbuf.unbindForBoth();

	_bloom.bindForBoth();
	_bloom.switchBuffer(1);
	renderer::ClearColor();
	_tmpbuf[0].active(0);
	_eff_bloom_blur_H.renderPURE();
	_bloom.unbindForBoth();

	/* ---- bloom combine ---- */
	_tmpbuf.bindForBoth();
	renderer::ClearColor();
	_bloom[0].active(0);
	_bloom[1].active(1);
	_eff_hdr_combine.renderPURE();
	_tmpbuf.unbindForBoth();

	renderer::SetViewPort(renderer::GetInnerX(), renderer::GetInnerY(), renderer::GetInnerW(), renderer::GetInnerH());
	renderer::BeginFrame(0, 0, 0, 0);
	_tmpbuf[0].active(0);
	_eff_fxaa.renderPURE();
	renderer::EndFrame();

	_last_mvp = org.comb();
	glEnable(GL_DEPTH_TEST);
}

///* ---- depth blur ---- */
//_bloom.bindForBoth();
//_bloom.switchBuffer(1);
//renderer::ClearColor();
//_tmpbuf[0].active(0);
//_eff_bloom_blur_H.renderPURE();
//_bloom.unbindForBoth();

//_bloom.bindForBoth();
//_bloom.switchBuffer(0);
//renderer::ClearColor();
//_bloom[1].active(0);
//_eff_bloom_blur_V.renderPURE();
//_bloom.unbindForBoth();

///* ---- depth combine ---- */
//_bloom.bindForBoth();
//_bloom.switchBuffer(1);
//renderer::SetClearColor(1.0f, 0.0f, 0.0f, 0.0f);
//renderer::ClearColor();
//{
//	glm::vec3 lookat = gbf.getPixel(0, 0.5f, 0.5f);
//	glm::vec3 dir = lookat - org.getEye();
//	float oneDivLen = glm::length(dir);
//	if (oneDivLen != 0.0f)
//		oneDivLen = 1.0f / oneDivLen;
//	_eff_depth_blur.shader().setfloat(_loc_depth_len, oneDivLen);
//	_eff_depth_blur.shader().setvec3(_loc_depth_eye, org.getEye());
//	_eff_depth_blur.shader().setvec3(_loc_depth_dir, dir * oneDivLen);
//}
//_tmpbuf[0].active(0);
//_bloom[0].active(1);
//gbf.pos_tex().active(2);
//_eff_depth_blur.renderPURE();
//_bloom.unbindForBoth();


//_final[0].bindForBoth();
//renderer::ClearColor();
//_tmpbuf[0].active(0);
//_eff_fxaa.renderPURE();
//_final[0].unbindForBoth();
//
//renderer::SetViewPort(renderer::GetInnerX(), renderer::GetInnerY(), renderer::GetInnerW(), renderer::GetInnerH());
//renderer::BeginFrame(0, 0, 0, 0);
//_final[0][0].active(0);
//gbf.pos_tex().active(1);
//_eff_motionBlur.shader().setmat4(_loc_last_mvp, _last_mvp);
//_eff_motionBlur.renderPURE();
//renderer::EndFrame();