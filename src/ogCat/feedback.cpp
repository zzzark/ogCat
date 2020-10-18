#include "feedback.h"
#include "dbg.h"
void cat::fbShader::end(const char* vars[], unsigned int count)
{
	CAT_CALL(glTransformFeedbackVaryings(shader::getProgram(), count, vars, GL_SEPARATE_ATTRIBS));
	shader::end();
}

void cat::fbShader::end(const char* var)
{
	const char* vars[] = { var, };
	CAT_CALL(glTransformFeedbackVaryings(shader::getProgram(), 1, vars, GL_SEPARATE_ATTRIBS));
	shader::end();
}

void cat::feedback::begin(FB_TYPE type)
{
	glEnable(GL_RASTERIZER_DISCARD);
	GLenum tp = 0;
	if (type == FB_TYPE::FB_POINTS)        tp = GL_POINTS;
	if (type == FB_TYPE::FB_LINES)         tp = GL_LINES;
	if (type == FB_TYPE::FB_TRIANGLES)     tp = GL_TRIANGLES;
	glBeginTransformFeedback(tp);
}

void cat::feedback::end()
{
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glFlush();
}

void cat::feedback::bind(const cat::vbo& obj, unsigned int slot)
{
	obj.bind();
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, slot, obj.getID());
}

void cat::feedback::unbind(const cat::vbo& obj)
{
	obj.unbind();
}
