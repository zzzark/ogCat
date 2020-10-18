#include "dbg.h"
#include "vertexBuffer.h"
cat::vao::vao()
{
	_id = 0;
}

cat::vao::~vao()
{
	release();
}

void cat::vao::begin()
{
	release();
	CAT_CALL(glGenVertexArrays(1, &_id));
	CAT_CALL(glBindVertexArray(_id));
	bind();
}

void cat::vao::addBuffer(const vbo& vbo, cat::VERTEX_TYPE type, int count, int layout)
{
	vbo.bind();
	CAT_CALL(glEnableVertexAttribArray(layout));
	switch (type) {
	case cat::VERTEX_TYPE::VT_FLOAT:
		CAT_CALL(glVertexAttribPointer(layout, count, GL_FLOAT, GL_FALSE, 0, nullptr));
		break;
	case cat::VERTEX_TYPE::VT_INT:
		CAT_CALL(glVertexAttribIPointer(layout, count, GL_INT, 0, nullptr));
		break;
	case cat::VERTEX_TYPE::VT_DOUBLE:
		CAT_CALL(glVertexAttribLPointer(layout, count, GL_FALSE, 0, nullptr));
		break;
	}
}

void cat::vao::end()
{
	unbind();
}

void cat::vao::bind() const
{
	glBindVertexArray(_id);
}

void cat::vao::unbind() const
{
	glBindVertexArray(0);
}

void cat::vao::release()
{
	if (_id) {
		CAT_CALL(glDeleteVertexArrays(1, &_id));
		_id = 0;
	}
}

cat::vbo::vbo()
{
	_id = 0;
}

cat::vbo::~vbo()
{
	release();
}

void cat::vbo::create(const void* data, unsigned int sizeInBytes)
{
	release();
	CAT_CALL(glGenBuffers(1, &_id));
	CAT_CALL(glBindBuffer(GL_ARRAY_BUFFER, _id));
	CAT_CALL(glBufferData(GL_ARRAY_BUFFER, sizeInBytes, data, GL_STATIC_DRAW));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void cat::vbo::create_dynamic(const void* data, unsigned int sizeInBytes)
{
	release();
	CAT_CALL(glGenBuffers(1, &_id));
	CAT_CALL(glBindBuffer(GL_ARRAY_BUFFER, _id));
	CAT_CALL(glBufferData(GL_ARRAY_BUFFER, sizeInBytes, data, GL_DYNAMIC_DRAW));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void cat::vbo::writeData_dynamic(const void* data, unsigned int offset, unsigned int sizeInBytes)
{
	bind();
	CAT_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, sizeInBytes, data));
	unbind();
}

void cat::vbo::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, _id);
}

void cat::vbo::unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void cat::vbo::release()
{
	if (_id) {
		glDeleteBuffers(1, &_id);
		_id = 0;
	}
}

cat::vibo::vibo()
{
	_id = 0;
	_count = 0;
}

cat::vibo::~vibo()
{
	glDeleteBuffers(1, &_id);
}

void cat::vibo::create(const unsigned int* data, unsigned int count)
{
	release();
	_count = count;
	CAT_CALL(glGenBuffers(1, &_id));
	CAT_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id));
	CAT_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

void cat::vibo::bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
}

void cat::vibo::unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void cat::vibo::release()
{
	if (_id) {
		glDeleteBuffers(1, &_id);
		_id = 0;
	}
}
