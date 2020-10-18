/*
@author:	zrk
@date:		2020/9/20
*/
#pragma once
#include "mesh.h"
#include "shader.h"
#include "texture2D.h"
#include "gbuffer.h"
#include "camera.h"
#include "material.h"

namespace cat
{
	class mesh_shader : protected shader {
	protected:
		int _loc_MVP = -1;
		int _loc_MDL = -1;
		int _loc_emm = -1;
		int _loc_dif = -1;
		int _loc_spe = -1;
		int _loc_shi = -1;
		int _loc_reg = -1;
	public:
		void create();
		void draw(const texture2D& tex, const material mtl, const glm::vec3& recg, const camera& cam, const glm::mat4& mdl, const mesh& ms) const;
		void bind()   const { shader::bind(); }
		void unbind() const { shader::unbind(); }

		inline static void prepareBuffer(const gbuffer& gbuf) {
			gbuf.switchBuffers(
				gbuffer::BUFFER_LAYER::NORMAL,
				gbuffer::BUFFER_LAYER::COLOR,
				gbuffer::BUFFER_LAYER::DIFFUSE,
				gbuffer::BUFFER_LAYER::SPECULAR,
				gbuffer::BUFFER_LAYER::RECONG,
				gbuffer::BUFFER_LAYER::EMMISIVE
			);
			renderer::ClearAll();
		}
	};
};