#include "mesh_shader.h"
#include "dbg.h"
static const char* vs =
"#version 450 core                                      \n"
"layout(location = 0) in vec3 position;                 \n"
"layout(location = 1) in vec2 uvcoords;                 \n"
"layout(location = 2) in vec3 normal;                   \n"
"                                                       \n"
"out vec2 v_uv;                                         \n"
"out vec3 v_pos;                                        \n"
"out vec3 v_nml;                                        \n"
"                                                       \n"
"uniform mat4 u_MVP;                                    \n"
"uniform mat4 u_MDL;                                    \n"
"                                                       \n"
"void main()                                            \n"
"{                                                      \n"
"    gl_Position = u_MVP * vec4(position, 1.0f);        \n"
"    v_uv = uvcoords;                                   \n"
"    v_pos = (u_MDL * vec4(position, 1.0f)).xyz;        \n"
"    v_nml = (u_MDL * vec4(normal, 0.0f)).xyz;          \n"
"}                                                      \n"
;

static const char* fs =
"#version 450 core                                      \n"
"in vec2 v_uv;                                          \n"
"in vec3 v_pos;                                         \n"
"in vec3 v_nml;                                         \n"
"uniform sampler2D u_tex;                               \n"
"uniform vec3  u_emm = vec3(0.2);                       \n"
"uniform vec3  u_dif = vec3(0.8);                       \n"
"uniform vec3  u_spe = vec3(0.8);                       \n"
"uniform float u_shi = 5.0;                             \n"
"uniform vec3  u_reg = vec3(0.0, 0.0, 0.0);             \n"
"layout(location = 0) out vec3 nml;                     \n"
"layout(location = 1) out vec3 col;                     \n"
"layout(location = 2) out vec3 dif;                     \n"
"layout(location = 3) out vec4 spe;                     \n"
"layout(location = 4) out vec3 reg;                     \n"
"layout(location = 5) out vec3 emm;                     \n"
"layout(location = 6) out vec3 pos;                     \n"
"                                                       \n"
"void main()                                            \n"
"{                                                      \n"
"    nml = v_nml;                                       \n"
"    col = texture(u_tex, v_uv).xyz;                    \n"
"    dif = u_dif;                                       \n"
"    spe = vec4(u_spe, u_shi);                          \n"
"    reg = u_reg;                                       \n"
"    emm = u_emm;                                       \n"
"    pos = v_pos;                                       \n"
"}                                                      \n"
;

void cat::defaultMeshShader::create()
{
	shader::begin();
	shader::load(vs, shader::SHADER_TYPE::VERTEX_SHADER);
	shader::load(fs, shader::SHADER_TYPE::FRAGMENT_SHADER);
	shader::end();

	_loc_MVP = shader::getlocation("u_MVP");
	_loc_MDL = shader::getlocation("u_MDL");
	_loc_emm = shader::getlocation("u_emm");
	_loc_dif = shader::getlocation("u_dif");
	_loc_spe = shader::getlocation("u_spe");
	_loc_shi = shader::getlocation("u_shi");
	_loc_reg = shader::getlocation("u_reg");

	shader::uniform tex = (*this)["u_tex"];
	tex = (int)(0);
}

void cat::defaultMeshShader::draw(const texture2D& tex, const material& mtl, const glm::vec3& recg, const camera& cam, const glm::mat4& mdl, const mesh& ms) const
{
	if (ms.getib().getCount() == 0)
		return;
	shader::setvec3(_loc_emm, mtl.emmisive);
	shader::setvec3(_loc_dif, mtl.diffuse);
	shader::setvec3(_loc_spe, mtl.specular);
	shader::setfloat(_loc_shi, mtl.shininess);
	shader::setvec3(_loc_reg, recg);
	shader::setmat4(_loc_MVP, cam.mvp(mdl));
	shader::setmat4(_loc_MDL, mdl);

	tex.active(0);
	ms.getvao().bind();
	ms.getib().bind();
	glDrawElements(GL_TRIANGLES, ms.getib().getCount(), GL_UNSIGNED_INT, nullptr);
}

void cat::defaultMeshShader::prepareBuffer(const gbuffer& gbuf)
{
	gbuf.bind();
	gbuf.switchBuffers(
		gbuffer::BUFFER_LAYER::NORMAL,
		gbuffer::BUFFER_LAYER::COLOR,
		gbuffer::BUFFER_LAYER::DIFFUSE,
		gbuffer::BUFFER_LAYER::SPECULAR,
		gbuffer::BUFFER_LAYER::RECONG,
		gbuffer::BUFFER_LAYER::EMMISIVE,
		gbuffer::BUFFER_LAYER::POSITION
	);
	//gbuf.bind();
	//gbuf.switchBuffers(
	//	gbuffer::BUFFER_LAYER::NORMAL,
	//	gbuffer::BUFFER_LAYER::EMMISIVE,
	//	gbuffer::BUFFER_LAYER::DIFFUSE,
	//	gbuffer::BUFFER_LAYER::SPECULAR,
	//	gbuffer::BUFFER_LAYER::RECONG,
	//	gbuffer::BUFFER_LAYER::COLOR
	//);
}
