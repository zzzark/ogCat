/*
@author:  zrk
@date:    2020/6/22
*/
#pragma once
#include "R2DLoader.h"
#include "mesh.h"
#include "texture2D.h"
namespace cat
{
	class terrainBlock : public static_mesh
	{
		unsigned int _width;
		unsigned int _height;

		void load(const char* filepath) {}
		void load(const char* ptr, size_t size) {}
		void load(const float* position, const float* uvCoords, const float* normal, const unsigned int* indices, unsigned int vtsCount, unsigned int idsCount) {}
	public:
		terrainBlock();

		void load(const char* filepath, float Yscale, float XZscale, float UVscale, float XOffset, float Zoffset, float Yoffset);
		void load(R2DLoader& ld, float Yscale, float XZscale, float UVscale, float XOffset, float Zoffset, float Yoffset);

		inline unsigned int getWidth() const { return _width; }
		inline unsigned int getHeight() const { return _height; }

	};
}