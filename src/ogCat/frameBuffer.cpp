#include "dbg.h"
#include "frameBuffer.h"

cat::frameBuffer::frameBuffer()
{
	_id = _index = _count = 0;
}

cat::frameBuffer::~frameBuffer()
{
	release();
}

void cat::frameBuffer::createEX(unsigned int width, unsigned int height, unsigned int count, unsigned int channel, unsigned int bit, bool bUseDepthBuffer, bool bUseStencilBuffer)
{
	release();
	_count = count + (bUseDepthBuffer || bUseStencilBuffer) ? 1 : 0;
	if (_count > frameBuffer::max_buffer_count) {
		std::cout << "error: multiBuffer::create: illegal parameter: count" << std::endl;
		return;
	}
	_index = count;
	CAT_CALL(glGenFramebuffers(1, &_id));
	CAT_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id));
	if (bUseDepthBuffer && !bUseStencilBuffer) {
		_dep.create(TEXTURE_TYPE::TT_DEPTH, width, height, TEXTURE_DATA_TYPE::TDT_FLOAT, nullptr, false);
		_dep.setfilter(TEXTURE_FILTER::TF_NEAREST);
		_dep.setwrap(TEXTURE_WRAP::TW_CLAMP_TO_EDGE);
		CAT_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _dep.getID(), 0));
		CAT_CALL(glDrawBuffer(GL_NONE));
		CAT_CALL(glReadBuffer(GL_NONE));
	}
	if (bUseStencilBuffer) {
		_stn.create(TEXTURE_TYPE::TT_DEPTH_STENCIL, width, height, TEXTURE_DATA_TYPE::TDT_FLOAT, nullptr, false);
		_stn.setfilter(TEXTURE_FILTER::TF_NEAREST);
		_stn.setwrap(TEXTURE_WRAP::TW_CLAMP_TO_EDGE);
		CAT_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _stn.getID(), 0));
		CAT_CALL(glDrawBuffer(GL_NONE));
		CAT_CALL(glReadBuffer(GL_NONE));
	}
	if (_index) {
		GLenum buffers[frameBuffer::max_buffer_count];
		for (unsigned int i = 0; i < _index; i++) {

			TEXTURE_TYPE ttp;
			switch (channel) {
			default:

			case 1:
				ttp = TEXTURE_TYPE::TT_R;
				break;
			case 2:
				ttp = TEXTURE_TYPE::TT_RG;
				break;
			case 3:
				ttp = TEXTURE_TYPE::TT_RGB;
				break;
			case 4:
				ttp = TEXTURE_TYPE::TT_RGBA;
				break;
			}
			//  ## difference ##  //
			_tex[i].createEX(
				ttp,
				bit,
				width, height, TEXTURE_DATA_TYPE::TDT_FLOAT, nullptr, false
			);
			_tex[i].setfilter(TEXTURE_FILTER::TF_NEAREST);
			_tex[i].setwrap(TEXTURE_WRAP::TW_CLAMP_TO_EDGE);
			CAT_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _tex[i].getID(), 0));
			buffers[i] = GL_COLOR_ATTACHMENT0 + i;
		}
		CAT_CALL(glDrawBuffers(_index, buffers));
	}
	CAT_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	CAT_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
}

void cat::frameBuffer::begin()
{
	release();
	CAT_CALL(glGenFramebuffers(1, &_id));
	CAT_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id));
}

void cat::frameBuffer::createSpec(bool isDepthBuffer, bool isDepthStencil, unsigned int width, unsigned int height, unsigned int channel, unsigned int bit)
{
	_count++;
	if (_count> frameBuffer::max_buffer_count) {
		std::cout << "error: multiBuffer::create: illegal parameter: count" << std::endl;
		return;
	}
	if (isDepthBuffer && !isDepthStencil) {
		_dep.create(TEXTURE_TYPE::TT_DEPTH, width, height, TEXTURE_DATA_TYPE::TDT_FLOAT, nullptr, false);
		_dep.setfilter(TEXTURE_FILTER::TF_NEAREST);
		_dep.setwrap(TEXTURE_WRAP::TW_CLAMP_TO_EDGE);
		CAT_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _dep.getID(), 0));
		CAT_CALL(glDrawBuffer(GL_NONE));
		CAT_CALL(glReadBuffer(GL_NONE));
	}
	else if (isDepthStencil) {
		_stn.create(TEXTURE_TYPE::TT_DEPTH_STENCIL, width, height, TEXTURE_DATA_TYPE::TDT_FLOAT, nullptr, false);
		_stn.setfilter(TEXTURE_FILTER::TF_NEAREST);
		_stn.setwrap(TEXTURE_WRAP::TW_CLAMP_TO_EDGE);
		CAT_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _stn.getID(), 0));
		CAT_CALL(glDrawBuffer(GL_NONE));
		CAT_CALL(glReadBuffer(GL_NONE));
	}
	else {
		_index++;
		TEXTURE_TYPE ttp;
		switch (channel) {
		case 1:
			ttp = TEXTURE_TYPE::TT_R;
			break;
		case 2:
			ttp = TEXTURE_TYPE::TT_RG;
			break;
		case 3:
			ttp = TEXTURE_TYPE::TT_RGB;
			break;
		default:

		case 4:
			ttp = TEXTURE_TYPE::TT_RGBA;
			break;
		}
		//  ## difference ##  //
		_tex[_index - 1].createEX(
			ttp,
			bit,
			width, height, TEXTURE_DATA_TYPE::TDT_FLOAT, nullptr, false
		);
		_tex[_index - 1].setfilter(TEXTURE_FILTER::TF_NEAREST);
		_tex[_index - 1].setwrap(TEXTURE_WRAP::TW_CLAMP_TO_EDGE);
		CAT_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _index - 1, GL_TEXTURE_2D, _tex[_index - 1].getID(), 0));
	}
}


void cat::frameBuffer::createUserDefined(bool isDepth, bool isDepthStencil, unsigned int id)
{
	_count++;
	if (_count > frameBuffer::max_buffer_count) {
		std::cout << "error: multiBuffer::create: illegal parameter: count" << std::endl;
		return;
	}
	GLenum attachment;
	if (isDepth && !isDepthStencil) {
		attachment = GL_DEPTH_ATTACHMENT;
	}
	else if (isDepthStencil) {
		attachment = GL_DEPTH_STENCIL_ATTACHMENT;
	}
	else {
		attachment = GL_COLOR_ATTACHMENT0 + _index;
		_index++;
	}
	CAT_CALL(glFramebufferTexture(GL_FRAMEBUFFER, attachment, id, 0));
}

void cat::frameBuffer::end()
{
	CAT_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	const GLenum buffers[] = {
		GL_COLOR_ATTACHMENT0 ,
		GL_COLOR_ATTACHMENT1 ,
		GL_COLOR_ATTACHMENT2 ,
		GL_COLOR_ATTACHMENT3 ,
		GL_COLOR_ATTACHMENT4 ,
		GL_COLOR_ATTACHMENT5 ,
		GL_COLOR_ATTACHMENT6 ,
		GL_COLOR_ATTACHMENT7 ,
		GL_COLOR_ATTACHMENT8 ,
		GL_COLOR_ATTACHMENT9 ,
		GL_COLOR_ATTACHMENT10,
		GL_COLOR_ATTACHMENT11,
		GL_COLOR_ATTACHMENT12,
		GL_COLOR_ATTACHMENT13,
		GL_COLOR_ATTACHMENT14,
		GL_COLOR_ATTACHMENT15,
	};
	if (_index == 0) {
		glDrawBuffer(GL_NONE);
	}
	else {
		glDrawBuffers(_index, buffers);
	}
	CAT_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
}


void cat::frameBuffer::create(unsigned int width, unsigned int height, unsigned int count, bool bUseDepthBuffer, bool bUseStencilBuffer)
{
	release();
	_count = count + (bUseDepthBuffer || bUseStencilBuffer) ? 1 : 0;
	if (_count > frameBuffer::max_buffer_count) {
		std::cout << "error: multiBuffer::create: illegal parameter: count" << std::endl;
		return;
	}
	_index = count;
	CAT_CALL(glGenFramebuffers(1, &_id));
	CAT_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id));
	if (bUseDepthBuffer && !bUseStencilBuffer) {
		_dep.create(TEXTURE_TYPE::TT_DEPTH, width, height, TEXTURE_DATA_TYPE::TDT_FLOAT, nullptr, false);
		_dep.setfilter(TEXTURE_FILTER::TF_NEAREST);
		_dep.setwrap(TEXTURE_WRAP::TW_CLAMP_TO_EDGE);
		CAT_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _dep.getID(), 0));
		CAT_CALL(glDrawBuffer(GL_NONE));
		CAT_CALL(glReadBuffer(GL_NONE));
	}
	if (bUseStencilBuffer) {
		_stn.create(TEXTURE_TYPE::TT_DEPTH_STENCIL, width, height, TEXTURE_DATA_TYPE::TDT_FLOAT, nullptr, false);
		_stn.setfilter(TEXTURE_FILTER::TF_NEAREST);
		_stn.setwrap(TEXTURE_WRAP::TW_CLAMP_TO_EDGE);
		CAT_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _stn.getID(), 0));
		CAT_CALL(glDrawBuffer(GL_NONE));
		CAT_CALL(glReadBuffer(GL_NONE));
	}
	if (_index) {
		GLenum buffers[frameBuffer::max_buffer_count];
		for (unsigned int i = 0; i < _index; i++) {
			_tex[i].create(TEXTURE_TYPE::TT_RGB, width, height, TEXTURE_DATA_TYPE::TDT_FLOAT, nullptr, false);
			_tex[i].setfilter(TEXTURE_FILTER::TF_NEAREST);
			_tex[i].setwrap(TEXTURE_WRAP::TW_CLAMP_TO_EDGE);
			CAT_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _tex[i].getID(), 0));
			buffers[i] = GL_COLOR_ATTACHMENT0 + i;
		}
		CAT_CALL(glDrawBuffers(_index, buffers));
	}
	CAT_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	CAT_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
}

void cat::frameBuffer::switchBuffers(unsigned int count) const
{
	const GLenum buffers[] = {
		GL_COLOR_ATTACHMENT0 ,
		GL_COLOR_ATTACHMENT1 ,
		GL_COLOR_ATTACHMENT2 ,
		GL_COLOR_ATTACHMENT3 ,
		GL_COLOR_ATTACHMENT4 ,
		GL_COLOR_ATTACHMENT5 ,
		GL_COLOR_ATTACHMENT6 ,
		GL_COLOR_ATTACHMENT7 ,
		GL_COLOR_ATTACHMENT8 ,
		GL_COLOR_ATTACHMENT9 ,
		GL_COLOR_ATTACHMENT10,
		GL_COLOR_ATTACHMENT11,
		GL_COLOR_ATTACHMENT12,
		GL_COLOR_ATTACHMENT13,
		GL_COLOR_ATTACHMENT14,
		GL_COLOR_ATTACHMENT15,
	};
	if (count == 0) {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	else if (count <= _index) {
		glDrawBuffers(count, buffers);
	}
}
void cat::frameBuffer::switchBuffer(unsigned int index) const
{
	if (index >= _index) {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	else {
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + index);
		glReadBuffer(GL_COLOR_ATTACHMENT0 + index);
	}
}
void cat::frameBuffer::bindForDrawing() const
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id);
}

void cat::frameBuffer::bindForReading() const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
}

void cat::frameBuffer::bindForBoth() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
}

void cat::frameBuffer::unbindForBoth() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void cat::frameBuffer::release()
{
	for (unsigned int i = 0; i < _index; i++) {
		_tex[i].release();
	}
	_dep.release();
	_stn.release();
	if (_id) {
		glDeleteFramebuffers(1, &_id);
		_id = 0;
	}
	_index = 0;
	_count = 0;
}

unsigned int cat::frameBuffer::sys_getMaxBufferCount()
{
	GLint imax;
	CAT_CALL(glGetIntegerv(GL_MAX_DRAW_BUFFERS, &imax));
	return (unsigned int)imax;
}

unsigned int cat::frameBuffer::sys_getDefinedBufferCount()
{
	return max_buffer_count;
}
