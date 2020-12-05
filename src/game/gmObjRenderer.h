#pragma once
#include "gmObject.h"
#include "../game/gmRenderer.h"

namespace cat
{
	class mesh_baker;
	class meshShader;
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
	};
}