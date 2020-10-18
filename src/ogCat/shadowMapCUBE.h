/*
@author:  zrk
@date:    2020/6/19
*/
#pragma once
#include "mesh.h"
#include "camera.h"
#include "shader.h"
#include "frameBuffer.h"
#include "textureCUBE.h"
#include "shadowBuffer.h"

namespace cat
{
	class shadowMapCUBE
	{
		textureCUBE _tex;
		frameBuffer _dep;
		shader _shd_genMap;
		int _loc_mdl;
		int _loc_mat;
		int _loc_lit;
		int _loc_far;
	public:
		shadowMapCUBE();
		~shadowMapCUBE();
		void create(unsigned int width, unsigned int height);

		void begin(const glm::vec3& point, float zNear, float zFar);
		void beginPURE(const glm::vec3& lightPos, float zNear, float zFar);
		void draw(const glm::mat4& world, const mesh& ms);
		void end();
		void endPURE();
		
		inline const textureCUBE& getDep() const { return _tex; }
		inline void release() {
			_dep.release();
			_tex.release();
			_shd_genMap.release();
		}
	};
	class shadowCUBEEffect final : protected frameEffect
	{
		int _loc_view;
		int _loc_color;
		int _loc_ambient;
		int _loc_brightness;
		int _loc_point;
		int _loc_viewInv;
		void create(const char*) {}
		void render(float r, float g, float b, float a, bool swap = true) const {}
		void renderPURE() const {}
	public:
		shadowCUBEEffect();
		void create();
		void applyEffect(shadowBuffer& shaBuf, const shadowMapCUBE& shaMap, const gbuffer& gbuf, const pointLight& lit, const camera& orgCam);
		void setBias(float bias) const;
		void setFarPlane(float zFar) const;
		void setMaxBrightness(float val) const;
	};
}