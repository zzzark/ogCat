#include <cmath>
#include "gmLight.h"
#include "ogCat/mesh_instance.h"
#include "ogCat/shadowBuffer.h"
#include "ogCat/shadowMapCUBE.h"
#include "ogCat/shadowMap2D.h"

void ogm::gmLight::defaultLightFunction(float& r, float& g, float& b, float& intensity, float& ar, float& ag, float& ab, float& maxval)
 {
	if (r < 0) r = 0;
	if (g < 0) g = 0;
	if (b < 0) b = 0;
	float len = std::sqrt(r * r + g * g + b * b);
	intensity = len / 1.73205f;
	r /= len;
	g /= len;
	b /= len;
	ar = r * intensity / 10.0f;
	ag = g * intensity / 10.0f;
	ab = b * intensity / 10.0f;
	if (ar > 0.707106f) ar = 0.707106f;
	if (ag > 0.707106f) ag = 0.707106f;
	if (ab > 0.707106f) ab = 0.707106f;
	maxval = intensity * 2.71828f;
}

ogm::gmLight::~gmLight() = default;

// ===================================================================================================== //

void ogm::gmPointLight::_refresh()
{
	if (_map && _eff && _pl) {
		_map->setParameters(_pl->position, _zNear, _zFar);
		_eff->setLightParameters(*_pl);
	}
}

void ogm::gmPointLight::create(unsigned int resolution, float zNear, float zFar)
{
	delete _pl;   _pl = nullptr;
	delete _map; _map = nullptr;
	delete _eff; _eff = nullptr;
	_pl = new cat::pointLight;
	_map = new cat::shadowMapCUBE;
	_eff = new cat::shadowCUBEEffect;
	_resolution = resolution;
	_zFar = zFar;
	_zNear = zNear;
	_map->create(_resolution, _resolution);
	_eff->create();
	_eff->setFarPlane(_zFar);
	_eff->setMaxBrightness(100.0f);
	move(0, 3, 0);
	setRGB(1, 1, 1);
}

void ogm::gmPointLight::setLightFunction(light_func new_func)
{
	_litFunc = !new_func ? gmLight::defaultLightFunction : new_func;
}

void ogm::gmPointLight::setRGB(float r, float g, float b)
{
	float i, ar, ag, ab, mv;
	_litFunc(r, g, b, i, ar, ag, ab, mv);
	if (_pl) {
		_pl->ambient.x = 0;//ar;
		_pl->ambient.y = 0;//ag;
		_pl->ambient.z = 0;//ab;
		_pl->color.x = r;
		_pl->color.y = g;
		_pl->color.z = b;
		_pl->intensity = i;
		_eff->setMaxBrightness(mv);
		_refresh();
	}
}

void ogm::gmPointLight::move(float x, float y, float z)
{
	if (_pl) {
		_pl->position.x = x;
		_pl->position.y = y;
		_pl->position.z = z;
		_refresh();
	}
}

void ogm::gmPointLight::sysSetBias(float bias)
{
	if (_eff)
		_eff->setBias(bias);
}

void ogm::gmPointLight::begin()
{
	cat::renderer::SetViewPort(0, 0, _resolution, _resolution);
	if(_map)
		_map->begin();
}

void ogm::gmPointLight::draw(const glm::mat4& mdl, const cat::mesh& ms)
{
	if(_map)
		_map->draw(mdl, ms);
}

void ogm::gmPointLight::draw(const ogm::gmObj& obj)
{
	draw(*obj.meshInstancePointer());
}

void ogm::gmPointLight::draw(const cat::meshInstance& inst)
{
	draw(inst.mdl, *inst.getMesh());
}

void ogm::gmPointLight::end()
{
	if(_map)
		_map->end();
}

void ogm::gmPointLight::apply(const gmSys& sys, const gmSurface& suf)
{
	if(_map && _eff)
		_eff->applyEffect(*suf.shadowBufferPointer(), *_map, *suf.gbufferPointer(), *sys.cameraPointer());
}

ogm::gmPointLight::~gmPointLight()
{
	delete _pl;   _pl = nullptr;
	delete _map; _map = nullptr;
	delete _eff; _eff = nullptr;

}

// ===================================================================================================== //

void ogm::gmDirectionalLight::_refresh()
{
	if (_map && _eff && _lit && _cam) {
		_eff->setParameters(*_cam, *_lit);
	}
}

void ogm::gmDirectionalLight::create(unsigned int resolution, float size, float depth)
{
	delete _cam; _cam = nullptr;
	delete _lit; _lit = nullptr;
	delete _map; _map = nullptr;
	delete _eff; _eff = nullptr;
	_size = size;
	_depth = depth;
	_resolution = resolution;

	_cam = new cat::camera;
	_lit = new cat::directionalLight;
	_map = new cat::shadowMap2D;
	_eff = new cat::shadow2DEffect;
	_cam->ortho(-size, size, -size, size, -depth, depth);
	_cam->walk(glm::vec3(1, 1, 1));
	_cam->look(glm::vec3(0, 0, 0));

	_lit->ambient = glm::vec3(0.1f, 0.1f, 0.1f);
	_lit->color = glm::vec3(0.3f, 0.3f, 0.3f);
	_lit->direction = glm::normalize(_cam->getAt() - _cam->getEye());
	_lit->intensity = 10.0f;

	_map->create(resolution, resolution);
	
	_eff->create();
	_eff->setBias(0.002f);
	_eff->setSize((float)resolution);

	move(0, 0, 0, 0);
	setRGB(1, 1, 1);
}

void ogm::gmDirectionalLight::setLightFunction(light_func new_func)
{
	_litFunc = !new_func ? gmLight::defaultLightFunction : new_func;
}

void ogm::gmDirectionalLight::move(float x, float y, float z, float theta)
{
	if (_cam) {
		_cam->walk(glm::vec3(x, y, z));
		_cam->look(glm::vec3(0, 0, 0));
		_lit->direction = glm::normalize(_cam->getAt() - _cam->getEye());
		_refresh();
	}
}

void ogm::gmDirectionalLight::setRGB(float r, float g, float b)
{
	float i, ar, ag, ab, mv;
	_litFunc(r, g, b, i, ar, ag, ab, mv);
	if (_lit) {
		_lit->ambient.x = ar;
		_lit->ambient.y = ag;
		_lit->ambient.z = ab;
		_lit->color.x = r;
		_lit->color.y = g;
		_lit->color.z = b;
		_lit->intensity = i;
		_refresh();
	}
}

void ogm::gmDirectionalLight::sysSetBias(float bias)
{
	if (_eff)
		_eff->setBias(bias);
}

void ogm::gmDirectionalLight::begin()
{
	if (_map) {
		cat::renderer::SetViewPort(0, 0, _resolution, _resolution);
		_map->begin();
	}
}

void ogm::gmDirectionalLight::draw(const glm::mat4& mdl, const cat::mesh& ms)
{
	if (_map && _cam)
		_map->draw(*_cam, mdl, ms);
}

void ogm::gmDirectionalLight::draw(const ogm::gmObj& obj)
{
	draw(*obj.meshInstancePointer());
}

void ogm::gmDirectionalLight::draw(const cat::meshInstance& inst)
{
	draw(inst.mdl, *inst.getMesh());
}

void ogm::gmDirectionalLight::end()
{
	if (_map) {
		_map->end();
	}
}

void ogm::gmDirectionalLight::apply(const gmSys& sys, const gmSurface& suf)
{
	_eff->applyEffect(*suf.shadowBufferPointer(), *_map, *suf.gbufferPointer(), *sys.cameraPointer());
}

ogm::gmDirectionalLight::~gmDirectionalLight()
{
	delete _cam; _cam = nullptr;
	delete _lit; _lit = nullptr;
	delete _map; _map = nullptr;
	delete _eff; _eff = nullptr;
}
