#include "gmWorld.h"
#include "ogCat/skybox.h"
#include "ogCat/terrain.h"
#include "ogCat/texture2D.h"
#include "ogCat/material.h"
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

void ogm::gmSky::release()
{
	_sky = nullptr;
}

void ogm::gmGround::create(const char* texFilepath, const char* filepath, float Yscale, float XZscale, float UVscale, float XOffset, float Zoffset, float Yoffset)
{
	_tex.reset(new cat::texture2D());
	_ground.reset(new cat::terrainBlock());
	_tex->createFromFile(texFilepath, true);
	_tex->setwrap(cat::TEXTURE_WRAP::TW_MIRRORED);
	_tex->setfilter(cat::TEXTURE_FILTER::TF_LINEAR, cat::TEXTURE_FILTER::TF_LINEAR);
	_ground->load(filepath, Yscale, XZscale, UVscale, XOffset, Zoffset, Yoffset);
}

void ogm::gmGround::renderTheGround(gmObjRenderer& objRenderer, gmSys& sys, gmSurface& suf)
{
	const cat::material mtl{ {0,0,0}, {1,1,1},{1,1,1},1 };
	if (_ground && _tex) {
		objRenderer.render(_ground.get(), _tex.get(), &mtl, glm::mat4(1.0f), glm::vec3(0.0f), suf, sys);
	}
}

void ogm::gmGround::release()
{
	_tex = nullptr;
	_ground = nullptr;
}
