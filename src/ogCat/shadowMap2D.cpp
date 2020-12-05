#include "shadowMap2D.h"
#include "common.h"

static const char* shadowMap2D_vs =
"#version 450 core                                      \n"
"layout(location = 0) in vec3 position;                 \n"
"uniform mat4 u_MVP;                                    \n"
"void main()                                            \n"
"{                                                      \n"
"    gl_Position = u_MVP * vec4(position, 1.0f);        \n"
"}                                                      \n"
;

static const char* shadowMap2D_fs =
"#version 450 core                                      \n"
"void main()                                            \n"
"{                                                      \n"
"}                                                      \n"
;


cat::shadowMap2D::shadowMap2D()
{
	_loc_mvp = -1;
}

cat::shadowMap2D::~shadowMap2D()
{
}

void cat::shadowMap2D::create(unsigned int width, unsigned int height)
{
	_shd_genMap.begin();
	_shd_genMap.load(shadowMap2D_vs, shader::SHADER_TYPE::VERTEX_SHADER);
	_shd_genMap.load(shadowMap2D_fs, shader::SHADER_TYPE::FRAGMENT_SHADER);
	_shd_genMap.end();
	_loc_mvp = _shd_genMap.getlocation("u_MVP");
	_dep.createEX(width, height, 0, 3U, 32U, true, false);
	_dep.getDep().bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	_dep.getDep().setwrap(cat::TEXTURE_WRAP::TW_CLAMP_TO_BORDER, 1.0f, 1.0f, 1.0f, 1.0f);
	_dep.getDep().unbind();
}

void cat::shadowMap2D::begin()
{
	_dep.bindForBoth();
	renderer::ClearDepth();
	_shd_genMap.bind();
}

void cat::shadowMap2D::beginPURE()
{
	_dep.bindForBoth();
	_shd_genMap.bind();
}

void cat::shadowMap2D::draw(const camera& lightCam, const glm::mat4& world, const mesh& ms)
{
	_shd_genMap.setmat4(_loc_mvp, lightCam.mvp(world));
	ms.getvao().bind();
	ms.getib().bind();
	glDrawElements(GL_TRIANGLES, ms.getib().getCount(), GL_UNSIGNED_INT, nullptr);
}

void cat::shadowMap2D::end()
{
	_shd_genMap.unbind();
	_dep.unbindForBoth();
}

void cat::shadowMap2D::endPURE()
{
	_shd_genMap.unbind();
	_dep.unbindForBoth();
}



static const char* ACC_SM_fs =
"#version 450 core                                                     \n"
"in vec2 v_uv;                                                         \n"
"uniform sampler2D u_lit;                                              \n"
"uniform sampler2D u_pos;                                              \n"
"uniform sampler2D u_nml;                                              \n"
"uniform sampler2D u_dif;                                              \n"
"uniform sampler2D u_spe;                                              \n"
"uniform sampler2DShadow u_DEP;                                        \n"
"uniform mat4 u_SM_mat4;                                               \n"
"uniform vec3 u_dir;                                                   \n"
"uniform vec3 u_view;                                                  \n"
"uniform float u_bias = 0.002;                                         \n"
"uniform float u_size = 2048.0;                                        \n"
"uniform vec3 u_color;                                                 \n"
"uniform vec3 u_ambient;                                               \n"
"uniform float u_brightness;                                           \n"
"uniform mat4 u_viewInv;                                               \n"
"layout(location = 0) out vec3 color;                                  \n"
"                                                                      \n"
"float calculateShadow(vec3 pos)                                       \n"
"{                                                                     \n"
"    vec4 lightVtx = u_SM_mat4 * vec4(pos, 1.0f);                      \n"
"    vec3 vtx = lightVtx.xyz / lightVtx.w;                             \n"
"    vtx = vtx * 0.5 + 0.5;                                            \n"
"    float curD = clamp(vtx.z - u_bias, 0.0, 1.0);                     \n"
"    float isLit = texture(u_DEP, vec3(vtx.xy, curD));                 \n"
"    if (isLit == 1.0) return 1.0;                                     \n"
"    float total = 0.0;                                                    \n"
"    for (int i = -1; i <= 1; i++) {                                       \n"
"    for (int j = -1; j <= 1; j++) {                                       \n"
"        vec3 UVC = vec3(vec2(i/u_size,j/u_size) + vtx.xy, curD);          \n"
"        total += texture(u_DEP, UVC);                                     \n"
"    }                                                                     \n"
"    }                                                                     \n"
"    return total * (1.0 / 9.0);                                           \n"
"}                                                                     \n"
"                                                                      \n"
"vec3 calculateLight(vec3 nml)                                         \n"
"{                                                                     \n"
"    float dotVal = max(dot(nml, u_dir), 0.0);                         \n"
"    vec3 h = normalize(u_view + u_dir);                               \n"
"    vec3 diffuse = texture(u_dif, v_uv).xyz;                          \n"
"    vec4 specular = texture(u_spe,v_uv);                              \n"
"    float HN = (dotVal == 0.0) ?                                      \n"
"                0.0 : pow(max(dot(h, nml), 0.0), specular.w);         \n"
"    return u_color * u_brightness                                     \n"
"           * (dotVal * diffuse + HN * specular.xyz);                  \n"
"}                                                                     \n"
"                                                                      \n"
//"vec3 getXYZ(vec2 uv, float d){                                        \n"
//"    vec4 clip;                                                        \n"
//"    clip.xy = uv * 2.0 - 1.0;                                         \n"
//"    clip.z  = d  * 2.0 - 1.0;                                         \n"
//"    clip.w  = 1.0;                                                    \n"
//"    vec4 fnl = u_viewInv * clip;                                      \n"
//"    return fnl.xyz / fnl.w;                                           \n"
//"}                                                                     \n"
//"                                                                      \n"
"void main()                                                           \n"
"{                                                                     \n"
//"    float d = texture(u_pos, v_uv).x;                                 \n"
//"    vec3 pos = getXYZ(v_uv, d);                                       \n"
"    vec3 pos = texture(u_pos, v_uv).xyz;                              \n"
"    vec3 nml = texture(u_nml, v_uv).xyz;                              \n"
"    color = texture(u_lit, v_uv).xyz                                  \n"
"          + calculateShadow(pos) * calculateLight(nml) + u_ambient;   \n"
"}                                                                     \n"
;

cat::shadow2DEffect::shadow2DEffect()
{
	_loc_SM_mat4 = -1;
	_loc_dir = -1;
	_loc_view = -1;
	_loc_color = -1;
	_loc_ambient = -1;
	_loc_brightness = -1;
	_loc_viewInv = -1;
}

void cat::shadow2DEffect::create()
{
	frameEffect::create(ACC_SM_fs);
	shader::uniform u_lit = frameEffect::shader()["u_lit"];
	shader::uniform u_pos = frameEffect::shader()["u_pos"];
	shader::uniform u_nml = frameEffect::shader()["u_nml"];
	shader::uniform u_dif = frameEffect::shader()["u_dif"];
	shader::uniform u_spe = frameEffect::shader()["u_spe"];
	shader::uniform u_DEP = frameEffect::shader()["u_DEP"];
	shader::uniform u_SM_mat4 = frameEffect::shader()["u_SM_mat4"];
	u_lit = (int)0;
	u_pos = (int)1;
	u_nml = (int)2;
	u_dif = (int)3;
	u_spe = (int)4;
	u_DEP = (int)5;
	_loc_SM_mat4    = frameEffect::shader().getlocation("u_SM_mat4");
	_loc_dir        = frameEffect::shader().getlocation("u_dir");
	_loc_view       = frameEffect::shader().getlocation("u_view");
	_loc_color      = frameEffect::shader().getlocation("u_color");
	_loc_ambient    = frameEffect::shader().getlocation("u_ambient");
	_loc_brightness = frameEffect::shader().getlocation("u_brightness");
	//_loc_viewInv    = frameEffect::shader().getlocation("u_viewInv");
}

void cat::shadow2DEffect::setParameters(const camera& litCam, const directionalLight& lit)
{
	frameEffect::shader().bind();
	frameEffect::shader().setmat4(_loc_SM_mat4, litCam.comb());
	frameEffect::shader().setvec3(_loc_dir, -glm::normalize(lit.direction));
	frameEffect::shader().setmat4(_loc_SM_mat4, litCam.comb());
	frameEffect::shader().setvec3(_loc_dir, -glm::normalize(lit.direction));
	frameEffect::shader().setvec3(_loc_color, lit.color);
	frameEffect::shader().setvec3(_loc_ambient, lit.ambient);
	frameEffect::shader().setfloat(_loc_brightness, lit.intensity);
}


void cat::shadow2DEffect::applyEffect(shadowBuffer& shaBuf, const shadowMap2D& shaMap, const gbuffer& gbuf, const camera& orgCam)
{
	frameEffect::shader().bind();
	frameEffect::shader().setvec3(_loc_view, glm::normalize(orgCam.getEye() - orgCam.getAt()));

	//glm::mat4 viewInv = glm::inverse(orgCam.comb());
	//frameEffect::shader().setmat4(_loc_viewInv, viewInv);
	shaBuf.begin_shadow();
	shaBuf.getTex().active(0);
	//gbuf.dep_tex().active(1);	// pos
	gbuf.pos_tex().active(1);	// @2020/11/4: fix bugs
	gbuf.nml_tex().active(2);
	gbuf.dif_tex().active(3);
	gbuf.spe_tex().active(4);
	shaMap.getDep().active(5);
	frameEffect::renderPURE();

	shaBuf.end_shadow();
	frameEffect::shader().unbind();
}

void cat::shadow2DEffect::setBias(float bias) const
{
	shader::uniform u_bias = frameEffect::shader()["u_bias"];
	u_bias = bias;
}

void cat::shadow2DEffect::setSize(float size) const
{
	shader::uniform u_size = frameEffect::shader()["u_size"];
	u_size = size;
}
