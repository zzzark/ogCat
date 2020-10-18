/*
@author:  zrk
@date:    2020/6/19
*/
#pragma once
#include "camera.h"
#include "frameBuffer.h"
#include "mesh.h"
#include "shader.h"
#include "renderer.h"
#include "shadowBuffer.h"
#include "gbuffer.h"
#include "material.h"

namespace cat
{
	class shadowMap2D
	{
	protected:
		frameBuffer _dep;
		shader _shd_genMap;
		int _loc_mvp;
	public:
		shadowMap2D();
		~shadowMap2D();
		void create(unsigned int width, unsigned int height);

		void begin();
		void beginPURE();
		void draw(const camera& lightCam, const glm::mat4& world, const mesh& ms);
		void end();
		void endPURE();

		inline const texture2D& getDep() const { return _dep.getDep(); }
		inline void release() {
			_dep.release();
			_shd_genMap.release();
		}
	};
	class shadow2DEffect final : protected frameEffect
	{
		int _loc_SM_mat4;
		int _loc_dir;
		int _loc_view;
		int _loc_color;
		int _loc_ambient;
		int _loc_brightness;
		int _loc_viewInv;
		void create(const char*) {}
		void render(float r, float g, float b, float a, bool swap = true) const {}
		void renderPURE() const {}
	public:
		shadow2DEffect();
		void create();
		void applyEffect(shadowBuffer& shaBuf, const shadowMap2D& shaMap, const gbuffer& gbuf, const directionalLight& lit, const camera& litCam, const camera& orgCam);
		void setBias(float bias) const;
		void setSize(float size) const;
	};
}