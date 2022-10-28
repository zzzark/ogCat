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
	class meshShader : protected shader
	{
	public:
		virtual void create() = 0;
		virtual void draw(
			const texture2D& tex, const material& mtl,
			const glm::vec3& recg, const camera& cam,
			const glm::mat4& mdl, const mesh& ms) const = 0;
		
		void bind()   const { shader::bind(); }
		void unbind() const { shader::unbind(); }

		virtual void prepareBuffer(const gbuffer& gbuf) = 0;
	};

	class defaultMeshShader : public meshShader
	{
	protected:
		int _loc_MVP = -1;
		int _loc_MDL = -1;
		int _loc_emm = -1;
		int _loc_dif = -1;
		int _loc_spe = -1;
		int _loc_shi = -1;
		int _loc_reg = -1;
	public:
		void create() override;
		void draw(
			const texture2D& tex, const material& mtl,
			const glm::vec3& recg, const camera& cam,
			const glm::mat4& mdl, const mesh& ms) const override;
		void prepareBuffer(const gbuffer& gbuf) override;
	};
};