#include "baker.h"

void cat::oven::create(const char* vs, const char* gs, const char* vars[], unsigned int count)
{
	_shd.begin();
	_shd.load(vs, cat::shader::SHADER_TYPE::VERTEX_SHADER);
	_shd.load(gs, cat::shader::SHADER_TYPE::GEOMETRY_SHADER);
	_shd.end(vars, count);
}

void cat::oven::begin()
{
	_shd.begin();
	feedback::begin(cat::feedback::FB_TYPE::FB_TRIANGLES);
}

void cat::oven::prepare(const vbo& dest, const vbo& src)
{
}

void cat::oven::bake()
{
}

void cat::oven::end()
{
}
