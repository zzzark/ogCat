#include "gmWorld.h"
#include "ogCat/skybox.h"
#include "ogCat/terrain.h"
#include "glm/gtc/matrix_transform.hpp"


void ogm::gmSky::create(const char* file1, const char* file2, const char* file3, const char* file4, const char* file5, const char* file6)
{
	_sky = nullptr;
	_sky.reset(new cat::skybox);
	_sky->createTEX(file1, file2, file3, file4, file5, file6);
}

ogm::gmSky::~gmSky()
{
	_sky = nullptr;
}

void ogm::gmSky::rotateY(float rad)
{
	self_transform = glm::rotate(glm::mat4(1.0f), rad, glm::vec3(0, 1, 0));
}

void ogm::gmSky::setIntensity(float val) const
{
	if(_sky.get())
		_sky->setIntensity(val);
}

void ogm::gmSky::renderTheSkey(gmSys& sys, gmSurface& suf)
{
	if (_sky) {
		_sky->getShader().prepareBuffer(*suf.gbufferPointer());
		//_sky->draw(*sys.cameraPointer());
		_sky->draw(sys.cameraPointer()->comb() * self_transform);
	}
}
