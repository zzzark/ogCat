#include "frameBuffer.h"

static const char* vs =
"#version 450 core                                      \n"
"layout(location = 0) in vec2 v_pos;                    \n"
"out vec2 v_uv;                                         \n"
"void main()                                            \n"
"{                                                      \n"
"    gl_Position = vec4(v_pos.x, v_pos.y, 0.0f, 1.0f);  \n"
"    v_uv = v_pos * 0.5f + vec2(0.5f);                  \n"
"}                                                      \n"
"                                                       \n"
;
cat::frameShader::frameShader()
{
}

cat::frameShader::~frameShader()
{
}

void cat::frameShader::createFS(const char* fs)
{
	shader::begin();
	shader::load(vs, SHADER_TYPE::VERTEX_SHADER);
	shader::load(fs, SHADER_TYPE::FRAGMENT_SHADER);
	shader::end();
}
