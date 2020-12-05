#include "dbg.h"
#include "skybox.h"
#include "R2DLoader.h"

static const char* vs =
"#version 450 core                                 \n"
"layout(location = 0) in vec3 position;            \n"
"                                                  \n"
"uniform mat4 u_MVP;                               \n"
"                                                  \n"
"out vec3 v_uvw;                                   \n"
"                                                  \n"
"void main()                                       \n"
"{                                                 \n"
"    vec4 pos = u_MVP * vec4(position, 0.0f);      \n"
"    gl_Position = pos.xyww;                       \n"
"    v_uvw = position;                             \n"
"}                                                 \n"
"                                                  \n"
;
static const char* fs =
"#version 450 core                                 \n"
"                                                  \n"
"in vec3 v_uvw;                                    \n"
"uniform samplerCube u_sky;                        \n"
"uniform float u_int = 1.0f;                       \n"
"out vec3 color;                                   \n"
"layout (depth_greater) out float gl_FragDepth;    \n"	// improve rendering performance
"void main()                                       \n"
"{                                                 \n"
"    color = texture(u_sky, v_uvw).xyz * u_int;    \n"
"    gl_FragDepth = 1.0f;                          \n"  // solve z-fighting
"}                                                 \n"

;
cat::skyshader::skyshader()
{
	shader::begin();
	shader::load(vs, SHADER_TYPE::VERTEX_SHADER);
	shader::load(fs, SHADER_TYPE::FRAGMENT_SHADER);
	shader::end();
	_mvp = shader::getlocation("u_MVP");
}

cat::skyshader::~skyshader()
{
}

void cat::skyshader::prepareBuffer(gbuffer& gbuf) const
{
	gbuf.bind();
	gbuf.switchBuffers(
		gbuffer::BUFFER_LAYER::EMMISIVE
	);
}

void cat::skyshader::setmat4(const glm::mat4& m) const
{
	shader::setmat4(_mvp, m);
}


const static float skyboxVertices[] = {
	 1,  1,  1,
	 1, -1,  1,
	 1,  1, -1,
	 1, -1, -1,
	-1,  1,  1,
	-1, -1,  1,
	-1,  1, -1,
	-1, -1, -1,
};
const static unsigned int skyboxIndices[] = {
	0, 1, 2, 1, 3, 2,	// right
	5, 4, 6, 5, 6, 7,	// left
	0, 2, 6, 0, 6, 4,	// top
	1, 5, 7, 1, 7, 3,	// bottom
	0, 5, 1, 0, 4, 5,	// front
	6, 2, 3, 3, 7, 6,	// back
};
cat::skybox::skybox() : _orig(1.0f)
{
	_bo.create(skyboxVertices, sizeof(skyboxVertices));
	_ib.create(skyboxIndices, sizeof(skyboxIndices) / sizeof(unsigned int));
	_ao.begin();
	_ao.addBuffer(_bo, VERTEX_TYPE::VT_FLOAT, 3, 0);
	_ao.end();
}

cat::skybox::~skybox()
{
}

void cat::skybox::setIntensity(float v)
{
	shader::uniform u_int = _shd["u_int"];
	u_int = v;
}

void cat::skybox::draw(const camera& cam)
{
	this->draw(cam.comb());
}

void cat::skybox::draw(const glm::mat4& cmb)
{
	GLint oldCull;
	GLint oldDepth;
	glGetIntegerv(GL_CULL_FACE_MODE, &oldCull);
	glGetIntegerv(GL_DEPTH_FUNC, &oldDepth);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	_shd.bind();
	_shd.setmat4(cmb * _orig);
	_ao.bind();
	_ib.bind();
	_tex.active();
	glDrawElements(GL_TRIANGLES, _ib.getCount(), GL_UNSIGNED_INT, nullptr);

	glCullFace(oldCull);
	glDepthFunc(oldDepth);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

}
