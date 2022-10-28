#pragma once
#include "gmObject.h"
#include "gmRenderer.h"

namespace cat
{
	class mesh_baker;
	class meshShader;
	class mesh;
	class texture2D;
	struct material;
}
namespace ogm
{
	class gmObjRenderer final : public gmRenderer
	{
	private:
		bool _is_created = false;
	private:
		cat::mesh_baker* _baker = nullptr;
		cat::meshShader* _shader = nullptr;
	public:
		void initialize();
		void terminate();
		~gmObjRenderer();

		void begin(gmSurface& suf) const;
		void end() const;
		void render(gmObj& obj, gmSurface& suf, gmSys& sys);
		void render(
			cat::mesh* ms, cat::texture2D* tex,
			const cat::material* mtl, const glm::mat4& mat, 
			const glm::vec3& rec, gmSurface& suf, gmSys& sys);
	};
}