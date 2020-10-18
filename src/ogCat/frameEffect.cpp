#include "dbg.h"
#include "frameEffect.h"

static float quard[] = {
	-1, -1,
	 1, -1,
	 1,  1,
	-1,  1,
};
static unsigned int indcs[] = {
	0, 1, 2,
	0, 2, 3,
};
cat::frameEffect::~frameEffect()
{
	release();
}

void cat::frameEffect::create(const char* fs)
{
	release();
	_shd.createFS(fs);
	_vbo.create(quard, sizeof(quard));
	_ibo.create(indcs, sizeof(indcs) / sizeof(unsigned int));
	_vao.begin();
	_vao.addBuffer(_vbo, VERTEX_TYPE::VT_FLOAT, 2, 0);
	_vao.end();
}
void cat::frameEffect::render(float r, float g, float b, float a, bool swap) const
{
	renderer::BeginFrame(r, g, b, a);
	_shd.bind();
	_vao.bind();
	_ibo.bind();
	glDrawElements(GL_TRIANGLES, _ibo.getCount(), GL_UNSIGNED_INT, nullptr);
	renderer::EndFrame(swap);
}

void cat::frameEffect::renderPURE() const
{
	_shd.bind();
	_vao.bind();
	_ibo.bind();
	glDrawElements(GL_TRIANGLES, _ibo.getCount(), GL_UNSIGNED_INT, nullptr);
}

void cat::frameEffect::release()
{
	_vao.release();
	_vbo.release();
	_ibo.release();
	_shd.release();
}
