/*
@author: zrk
@date:	 2020/11/11
*/

#pragma once
#include "../game/gmRenderer.h"
#include <memory>

namespace cat
{
	class mesh;
	class skybox;
}

namespace ogm
{
	class gmSky final
	{
		std::shared_ptr<cat::skybox> _sky;
	public:
		glm::mat4 self_transform;
	public:
		gmSky() { self_transform = glm::mat4(1.0f); }

		void create(
			const char* file1, const char* file2, const char* file3, 
			const char* file4, const char* file5, const char* file6);
		~gmSky();
		
		void rotateY(float rad);
		void setIntensity(float val) const;
		void renderTheSkey(gmSys& sys, gmSurface& suf);
	};
}