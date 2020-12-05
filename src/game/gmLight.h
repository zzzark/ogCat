/*
//@author:  zrk
//@date:    2020/10/18
*/

#pragma once
#include "../game/gmSys.h"
#include "../game/gmObject.h"

namespace cat
{
	class shadowBuffer;
	class meshInstance;
	class mesh;
	class camera;
	struct pointLight;
	class shadowMapCUBE;
	class shadowCUBEEffect;
	class shadowMap2D;
	class shadow2DEffect;
	struct directionalLight;
}
namespace ogm
{
	class gmLight
	{
	public:
		typedef void (*light_func)(float& r, float& g, float& b, float& intensity, float& ar, float& ag, float& ab, float& maxval);
		static void defaultLightFunction(float& r, float& g, float& b, float& intensity, float& ar, float& ag, float& ab, float& maxval);
	public:
		virtual void begin() = 0 {};
		virtual void draw(const gmObj& obj) = 0 {};
		virtual void draw(const cat::meshInstance& inst) = 0 {};
		virtual void draw(const glm::mat4& mdl, const cat::mesh& ms) = 0 {};
		virtual void end() = 0 {};

		virtual void apply(const gmSys& sys, const gmSurface& suf) = 0 {};
		virtual ~gmLight() = 0 {};
	};

	class gmPointLight final : public gmLight
	{
		gmPointLight(gmPointLight&) = delete;
		void operator=(gmPointLight&) = delete;

		gmLight::light_func _litFunc = gmLight::defaultLightFunction;
		float _zFar = 0;
		float _zNear = 0;
		unsigned int _resolution = 0;
		cat::pointLight* _pl = nullptr;
		cat::shadowMapCUBE* _map = nullptr;
		cat::shadowCUBEEffect* _eff = nullptr;
		void _refresh();
	public:
		gmPointLight() {}

		void create(unsigned int resolution = 1024, float zNear = 0.1f, float zFar = 1000.0f);
		void setLightFunction(light_func new_func);
		void move(float x, float y, float z);
		void setRGB(float r, float g, float b);

		// note:
		//	0.00005f is a pretty reasonable bias
		//	in the situation that 1.0f equals to one meter
		//
		void sysSetBias(float bias = 0.00005f);
		virtual void begin() override;
		virtual void draw(const gmObj& obj) override;
		virtual void draw(const cat::meshInstance& inst) override;
		virtual void draw(const glm::mat4& mdl, const cat::mesh& ms) override;
		virtual void end() override;

		virtual void apply(const gmSys& sys, const gmSurface& suf) override;
		virtual ~gmPointLight();

	};

	class gmDirectionalLight final : public gmLight
	{
		gmDirectionalLight(gmDirectionalLight&) = delete;
		void operator=(gmDirectionalLight&) = delete;

		gmLight::light_func _litFunc = gmLight::defaultLightFunction;
		float _size = 0.0f;
		float _depth = 0.0f;
		unsigned int _resolution = 0;
		cat::camera *_cam = nullptr;
		cat::shadowMap2D *_map = nullptr;
		cat::shadow2DEffect *_eff = nullptr;
		cat::directionalLight *_lit = nullptr;
		void _refresh();
	public:
		gmDirectionalLight() {};
		void create(unsigned int resolution = 1024, float size = 10.0f, float depth = 10.0f);
		void setLightFunction(light_func new_func);
		void move(float x, float y, float z, float theta);
		void setRGB(float r, float g, float b);

		// note:
		//	0.002f is a pretty reasonable bias
		//	in the situation that 1.0f equals to one meter
		//
		void sysSetBias(float bias = 0.002f);
		virtual void begin() override;
		virtual void draw(const gmObj& obj) override;
		virtual void draw(const cat::meshInstance& inst) override;
		virtual void draw(const glm::mat4& mdl, const cat::mesh& ms) override;
		virtual void end() override;

		virtual void apply(const gmSys& sys, const gmSurface& suf) override;
		virtual ~gmDirectionalLight();
	};
}