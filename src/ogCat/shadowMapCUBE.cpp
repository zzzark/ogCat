#include "shadowMapCUBE.h"
#include "dbg.h"
#include "camera.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static cat::camera g_cam;

static const char* shadowMapCUBE_vs = 
"#version 450 core                                      \n"
"layout(location = 0) in vec3 position;                 \n"
"uniform mat4 u_MDL;                                    \n"
"void main()                                            \n"
"{                                                      \n"
"    gl_Position = u_MDL * vec4(position, 1.0f);        \n"
"}                                                      \n"
;
static const char* shadowMapCUBE_gs =
"                                                                      \n"
"#version 330 core                                                     \n"
"layout(triangles) in;                                                 \n"
"layout(triangle_strip, max_vertices = 18) out;                        \n"
"uniform mat4 u_mat[6];                                                \n"
"out vec4 v_pos;                                                       \n"
"void main()                                                           \n"
"{                                                                     \n"
"    for (int face = 0; face < 6; ++face)                              \n"
"    {                                                                 \n"
"        gl_Layer = face;                                              \n"
"        for (int i = 0; i < 3; ++i)                                   \n"
"        {                                                             \n"
"            v_pos = gl_in[i].gl_Position;                             \n"
"            gl_Position = u_mat[face] * v_pos;                        \n"
"            EmitVertex();                                             \n"
"        }                                                             \n"
"        EndPrimitive();                                               \n"
"    }                                                                 \n"
"}                                                                     \n"
;
static const char* shadowMapCUBE_fs = 
"                                                                      \n"
"#version 330 core                                                     \n"
"in vec4 v_pos;                                                        \n"
"                                                                      \n"
"uniform vec3 u_lit;                                                   \n"
"uniform float u_far;                                                  \n"
"                                                                      \n"
"void main()                                                           \n"
"{                                                                     \n"
"    float lightDistance = length(v_pos.xyz - u_lit);                  \n"
"    lightDistance = lightDistance / u_far;                            \n"
"    gl_FragDepth = lightDistance;                                     \n"
"}                                                                     \n"
;

cat::shadowMapCUBE::shadowMapCUBE()
{
	g_cam.perspective(0.1f, 100.0f, 3.1415926f / 2.0f, 1.0f);
	_loc_lit = _loc_far = _loc_mat = _loc_mdl = -1;
}

cat::shadowMapCUBE::~shadowMapCUBE()
{
}

void cat::shadowMapCUBE::create(unsigned int width, unsigned int height)
{
	_shd_genMap.begin();
	_shd_genMap.load(shadowMapCUBE_vs, shader::SHADER_TYPE::VERTEX_SHADER);
	_shd_genMap.load(shadowMapCUBE_gs, shader::SHADER_TYPE::GEOMETRY_SHADER);
	_shd_genMap.load(shadowMapCUBE_fs, shader::SHADER_TYPE::FRAGMENT_SHADER);
	_shd_genMap.end();
	_loc_mdl = _shd_genMap.getlocation("u_MDL");
	_loc_mat = _shd_genMap.getlocation("u_mat[0]");
	_loc_lit = _shd_genMap.getlocation("u_lit");
	_loc_far = _shd_genMap.getlocation("u_far");

	_tex.begin();
	for (int i = 0; i < 6; i++) {
		_tex.createFaceEX(i, width, height, 1U, 16U, TEXTURE_TYPE::TT_DEPTH, TEXTURE_DATA_TYPE::TDT_FLOAT, nullptr);
	}
	_tex.end();
	_tex.setfilter(TEXTURE_FILTER::TF_NEAREST);
	_tex.setwrap(TEXTURE_WRAP::TW_CLAMP_TO_EDGE);

	_dep.begin();
	_dep.createUserDefined(true, false, _tex.getID());
	_dep.end();

	_tex.bind();
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	_tex.unbind();
}

void cat::shadowMapCUBE::begin(const glm::vec3& lightPos, float zNear, float zFar)
{
	_dep.bindForBoth();
	renderer::ClearDepth();
	_shd_genMap.bind();
	_shd_genMap.setvec3(_loc_lit, lightPos);
	_shd_genMap.setfloat(_loc_far, zFar);
	float aspect = 1.0f;
	float near = zNear;
	float far = zFar;
	glm::mat4 shadowProj = glm::perspective(3.1415926f / 2.0f, aspect, near, far);
	glm::mat4 shadowTransforms[6];
	shadowTransforms[0] = (shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms[1] = (shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms[2] = (shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms[3] = (shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms[4] = (shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms[5] = (shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
	for (int i = 0; i < 6; i++) {
		_shd_genMap.setmat4(_loc_mat + i, shadowTransforms[i]);
	}
}

void cat::shadowMapCUBE::beginPURE(const glm::vec3& lightPos, float zNear, float zFar)
{
	_dep.bindForBoth();
	_shd_genMap.bind();

	float aspect = 1.0f;
	float near = zNear;
	float far = zFar;
	glm::mat4 shadowProj = glm::perspective(3.1415926f / 2.0f, aspect, near, far);
	glm::mat4 shadowTransforms[6];
	shadowTransforms[0] = (shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms[1] = (shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms[2] = (shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms[3] = (shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms[4] = (shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms[5] = (shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
	for (int i = 0; i < 6; i++) {
		_shd_genMap.setmat4(_loc_mat + i, shadowTransforms[i]);
	}
}

void cat::shadowMapCUBE::draw(const glm::mat4& world, const mesh& ms)
{
	_shd_genMap.setmat4(_loc_mdl, world);
	ms.getvao().bind();
	ms.getib().bind();
	glDrawElements(GL_TRIANGLES, ms.getib().getCount(), GL_UNSIGNED_INT, nullptr);
}

void cat::shadowMapCUBE::end()
{
	_shd_genMap.unbind();
	_dep.unbindForBoth();
}

void cat::shadowMapCUBE::endPURE()
{
	_shd_genMap.unbind();
	_dep.unbindForBoth();
}


static const char* ACC_SM_fs =
"#version 450 core                                                     \n"
"in vec2 v_uv;                                                         \n"
"                                                                      \n"
"uniform float u_MaxBrightness = 10000.0f;                             \n"
"uniform vec3 u_view;                                                  \n"
"uniform vec3 u_color;                                                 \n"
"uniform vec3 u_ambient;                                               \n"
"uniform float u_brightness;                                           \n"
"uniform vec3 u_litPos;                                                \n"
"uniform float u_OneDivFar;                                            \n"
"uniform float u_bias = 0.00005f;                                      \n"
"uniform mat4 u_viewInv;                                               \n"
"                                                                      \n"
"uniform samplerCubeShadow u_sha;                                      \n"
"uniform sampler2D u_lit;                                              \n"
"uniform sampler2D u_pos;                                              \n"
"uniform sampler2D u_nml;                                              \n"
"uniform sampler2D u_dif;                                              \n"
"uniform sampler2D u_spe;                                              \n"
"                                                                      \n"
"layout(location = 0) out vec3 color;                                  \n"
"float calculateShadow(vec3 pos)                                       \n"
"{                                                                     \n"
"    vec3 dir = pos - u_litPos;                                        \n"
"    vec4 smp = vec4(dir, length(dir) * u_OneDivFar - u_bias);         \n"
"    return texture(u_sha, smp);                                       \n"
//"    float closestDepth = texture(u_sha, dir).r;                       \n"
//"    closestDepth *= u_far;                                            \n"
//"    float currentDepth = length(dir);                                 \n"
//"    float total = currentDepth - u_bias < closestDepth ? 1.0 : 0.0;   \n"
//"    return total;                                                     \n"
"}                                                                     \n"
"vec3 calculateLight(vec3 p2p, vec3 nml)                               \n"
"{                                                                     \n"
"    vec3 dir = normalize(p2p);                                        \n"
"    float dotVal = max(dot(nml, dir), 0.0);                           \n"
"    vec3 h = normalize(u_view + dir);                                 \n"
"    vec3 diffuse = texture(u_dif, v_uv).xyz;                          \n"
"    vec4 specular = texture(u_spe,v_uv);                              \n"
"    float HN = (dotVal == 0.0) ?                                      \n"
"                0.0 : pow(max(dot(h, nml), 0.0), specular.w);         \n"
"    float factor = 1.0 / max(dot(p2p, p2p), 0.00001);                 \n"
"    return u_color * min(factor * u_brightness, u_MaxBrightness)      \n"
"           * (dotVal * diffuse + HN * specular.xyz);                  \n"
"}                                                                     \n"
"vec3 getXYZ(vec2 uv, float d){                                        \n"
"    vec4 clip;                                                        \n"
"    clip.xy = uv * 2.0 - 1.0;                                         \n"
"    clip.z  = d  * 2.0 - 1.0;                                         \n"
"    clip.w  = 1.0;                                                    \n"
"    vec4 fnl = u_viewInv * clip;                                      \n"
"    return fnl.xyz / fnl.w;                                           \n"
"}                                                                     \n"
"void main()                                                           \n"
"{                                                                     \n"
"    float d = texture(u_pos, v_uv).x;                                 \n"
"    vec3 pos = getXYZ(v_uv, d);                                       \n"
"    vec3 nml = texture(u_nml, v_uv).xyz;                              \n"
"    vec3 p2p = u_litPos - pos;                                        \n"
"    color = texture(u_lit, v_uv).xyz +                                \n"
"         calculateShadow(pos) * calculateLight(p2p, nml) + u_ambient; \n"
//"    ((dot(p2p, p2p) > u_brightness) ?                                 \n"
//"         vec3(0.0) :                                                  \n"
//"         calculateShadow(pos) * calculateLight(p2p, nml) + u_ambient  \n"
//"    );                                                                \n"
"}                                                                     \n"
;
cat::shadowCUBEEffect::shadowCUBEEffect()
{
	_loc_view       = -1;
	_loc_color      = -1;
	_loc_ambient    = -1;
	_loc_brightness = -1;
	_loc_point      = -1;
	_loc_viewInv    = -1;
}

void cat::shadowCUBEEffect::create()
{
	frameEffect::create(ACC_SM_fs);
	shader::uniform u_sha = frameEffect::shader()["u_sha"];
	shader::uniform u_lit = frameEffect::shader()["u_lit"];
	shader::uniform u_pos = frameEffect::shader()["u_pos"];
	shader::uniform u_nml = frameEffect::shader()["u_nml"];
	shader::uniform u_dif = frameEffect::shader()["u_dif"];
	shader::uniform u_spe = frameEffect::shader()["u_spe"];
	u_sha = (int)0;
	u_lit = (int)1;
	u_pos = (int)2;
	u_nml = (int)3;
	u_dif = (int)4;
	u_spe = (int)5;
	_loc_view       = frameEffect::shader().getlocation("u_view");
	_loc_color      = frameEffect::shader().getlocation("u_color");
	_loc_ambient    = frameEffect::shader().getlocation("u_ambient");
	_loc_brightness = frameEffect::shader().getlocation("u_brightness");
	_loc_point      = frameEffect::shader().getlocation("u_litPos");
	_loc_viewInv    = frameEffect::shader().getlocation("u_viewInv");
}

void cat::shadowCUBEEffect::applyEffect(shadowBuffer& shaBuf, const shadowMapCUBE& shaMap, const gbuffer& gbuf, const pointLight& lit, const camera& orgCam)
{
	frameEffect::shader().bind();
	frameEffect::shader().setvec3(_loc_view, glm::normalize(orgCam.getEye() - orgCam.getAt()));
	frameEffect::shader().setvec3(_loc_color, lit.color);
	frameEffect::shader().setvec3(_loc_ambient, lit.ambient);
	frameEffect::shader().setvec3(_loc_point, lit.position);
	frameEffect::shader().setfloat(_loc_brightness, lit.intensity);
	glm::mat4 viewInv = glm::inverse(orgCam.comb());
	frameEffect::shader().setmat4(_loc_viewInv, viewInv);

	shaBuf.begin_shadow();
	shaMap.getDep().active(0);
	shaBuf.getTex().active(1);
	gbuf.dep_tex().active(2);
	gbuf.nml_tex().active(3);
	gbuf.dif_tex().active(4);
	gbuf.spe_tex().active(5);
	frameEffect::renderPURE();
	shaBuf.end_shadow();
}

void cat::shadowCUBEEffect::setBias(float bias) const
{
	shader::uniform u_bias = frameEffect::shader()["u_bias"];
	u_bias = bias;
}

void cat::shadowCUBEEffect::setFarPlane(float zFar) const
{
	if (zFar == 0.0f)
		return;
	shader::uniform u_OneDivFar = frameEffect::shader()["u_OneDivFar"];
	u_OneDivFar = 1.0f / zFar;
}

void cat::shadowCUBEEffect::setMaxBrightness(float val) const
{
	if (val == 0.0f)
		return;
	shader::uniform u_MaxBrightness = frameEffect::shader()["u_MaxBrightness"];
	u_MaxBrightness = val;
}
