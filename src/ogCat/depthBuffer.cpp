#include "dbg.h"
#include "frameBuffer.h"


cat::depthBuffer::depthBuffer()
{
	_id = 0;
}

cat::depthBuffer::~depthBuffer()
{
	release();
}

void cat::depthBuffer::create(unsigned int width, unsigned int height)
{
	release();
	CAT_CALL(glGenFramebuffers(1, &_id));
	CAT_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id));
	// ---------------------------------------------------------------------------------------//
	_dep.create(TEXTURE_TYPE::TT_DEPTH, width, height, TEXTURE_DATA_TYPE::TDT_FLOAT, nullptr, false);
	_dep.setfilter(TEXTURE_FILTER::TF_NEAREST);
	CAT_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _dep.getID(), 0));
	CAT_CALL(glDrawBuffer(GL_NONE));
	CAT_CALL(glReadBuffer(GL_NONE));
	CAT_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	CAT_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
}

void cat::depthBuffer::bindForDrawing()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id);
}

void cat::depthBuffer::bindForReading()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
}

void cat::depthBuffer::bindForBoth()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
}

void cat::depthBuffer::unbindForBoth()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void cat::depthBuffer::release()
{
	if (_id) {
		glDeleteFramebuffers(1, &_id);
		_id = 0;
	}
	_dep.release();
}
