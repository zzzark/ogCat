#include "dbg.h"
#include "frameBuffer.h"

cat::standardBuffer::standardBuffer()
{
	_id = 0;
}

cat::standardBuffer::~standardBuffer()
{
	release();
}

void cat::standardBuffer::create(unsigned int width, unsigned int height, bool bUseDepthBuffer, bool alphaChannel)
{
	release();
	CAT_CALL(glGenFramebuffers(1, &_id));
	CAT_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id));
	// ---------------------------------------------------------------------------------------//
	_tex.create(
		alphaChannel ? TEXTURE_TYPE::TT_RGBA : TEXTURE_TYPE::TT_RGB,
		width, height, TEXTURE_DATA_TYPE::TDT_FLOAT, nullptr, false
	);
	_tex.setfilter(TEXTURE_FILTER::TF_NEAREST);
	_tex.setwrap(TEXTURE_WRAP::TW_CLAMP_TO_EDGE);
	CAT_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tex.getID(), 0));
	// ---------------------------------------------------------------------------------------//
	if (bUseDepthBuffer) {
		_dep.create(TEXTURE_TYPE::TT_DEPTH, width, height, TEXTURE_DATA_TYPE::TDT_FLOAT, nullptr, false);
		_dep.setfilter(TEXTURE_FILTER::TF_NEAREST);
		_dep.setwrap(TEXTURE_WRAP::TW_CLAMP_TO_EDGE);
		CAT_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _dep.getID(), 0));
	}

	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	CAT_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	CAT_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
}

void cat::standardBuffer::bindForBoth()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
}

void cat::standardBuffer::bindForDrawing()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id);
}

void cat::standardBuffer::bindForReading()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
}

void cat::standardBuffer::unbindForBoth()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void cat::standardBuffer::blit(unsigned int winLeft, unsigned int winTop, unsigned int winRight, unsigned int winBottom, TEXTURE_FILTER filter)
{
	if (!_id) return;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
	CAT_CALL(glBlitFramebuffer(
		0, 0, _tex.getWidth(), _tex.getHeight(), 
		winLeft, winTop, winRight, winBottom, 
		GL_COLOR_BUFFER_BIT, filter == TEXTURE_FILTER::TF_NEAREST ? GL_NEAREST : GL_LINEAR
	));
}
void cat::standardBuffer::release()
{
	if (_id) {
		glDeleteFramebuffers(1, &_id);
		_id = 0;
	}
	_tex.release();
	_dep.release();
}
