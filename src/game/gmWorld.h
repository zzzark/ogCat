/*
@author: zrk
@date:	 2020/11/11
*/

#pragma once
#include "gmRenderer.h"
#include "gmObjRenderer.h"
#include "gmObjRenderer.h"
#include <memory>

namespace cat
{
	class mesh;
	class skybox;
	class terrainBlock;
	class texture2D;
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

		void release();
	};

	class gmGround final
	{
		std::shared_ptr<cat::texture2D> _tex;
		std::shared_ptr<cat::terrainBlock> _ground;
	public:
		void create(const char* texFilepath, const char* filepath, float Yscale, float XZscale, float UVscale, float XOffset, float Zoffset, float Yoffset);
		void renderTheGround(gmObjRenderer& objRenderer, gmSys& sys, gmSurface& suf);

		void release();
	};
}