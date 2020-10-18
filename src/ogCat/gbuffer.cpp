#include "gbuffer.h"
#include "renderer.h"
#include "dbg.h"

void cat::gbuffer::create(unsigned int width, unsigned int height)
{
	_gbuffer.begin();
	_gbuffer.createSpec( true, false, width, height, 1U, 16U);  // dep
	_gbuffer.createSpec(false, false, width, height, 3U, 16U);	// nml 0
	_gbuffer.createSpec(false, false, width, height, 3U, 16U);	// col 1
	_gbuffer.createSpec(false, false, width, height, 3U, 16U);	// dif 2
	_gbuffer.createSpec(false, false, width, height, 4U, 16U);	// spe 3
	_gbuffer.createSpec(false, false, width, height, 3U, 16U);	// reg 4
	_gbuffer.createSpec(false, false, width, height, 3U, 16U);	// emm 5
	_gbuffer.end();

	_width  = width;
	_height = height;
}


const glm::vec3 cat::gbuffer::getPixel(unsigned int index, float x, float y) const
{
	_gbuffer.bindForBoth();
	_gbuffer.switchBuffer(index);
	int xx = (int)((float)_gbuffer.getTex(1).getWidth()  * x);
	int yy = (int)((float)_gbuffer.getTex(1).getHeight() * y);
	float p[3];
	CAT_CALL(glReadPixels(xx, yy, 1, 1, GL_RGB, GL_FLOAT, p));
	_gbuffer.switchBuffers(7);
	_gbuffer.unbindForBoth();
	return glm::vec3(p[0], p[1], p[2]);
}

const glm::vec3 cat::gbuffer::getRecognizationPixel(float x, float y) const
{
	_gbuffer.bindForBoth();
	_gbuffer.switchBuffer(gbuffer::BUFFER_LAYER::RECONG);
	int xx = (int)((float)_gbuffer.getTex(1).getWidth()  * x);
	int yy = (int)((float)_gbuffer.getTex(1).getHeight() * y);
	float p[3];
	CAT_CALL(glReadPixels(xx, yy, 1, 1, GL_RGB, GL_FLOAT, p));
	_gbuffer.switchBuffers(7);
	_gbuffer.unbindForBoth();
	return glm::vec3(p[0], p[1], p[2]);
}

const void cat::gbuffer::switchBuffers(int so, int s1, int s2, int s3, int s4, int s5, int s6, int s7) const
{
	GLenum buffers[8];
	int params[] = { so,s1,s2,s3,s4,s5,s6,s7 };
	unsigned int i = 0;
	for (; i < 8; i++) {
		if (params[i] < 0)
			break;
		else
			buffers[i] = GL_COLOR_ATTACHMENT0 + params[i];
	}
	if (i == 0) {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	else {
		glDrawBuffers(i, buffers);
	}
}