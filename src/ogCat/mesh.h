/*
//@author:	zark
//@date:	2020/6/12
//note:
//convention:
// layout(location = 0) in vec3 position; <==> _xyz
// layout(location = 1) in vec2 uvCoords; <==> _uv
// layout(location = 2) in vec3 normal;   <==> _str
//
//

*/

#pragma once
#include "R3DLoader.h"
#include "vertexBuffer.h"
#include <iostream>

namespace cat
{
	class mesh
	{
	protected:
		vao _vao;
		vbo _xyz;
		vbo _uv;
		vbo _str;
		vibo _ib;
	public:
		inline const vao& getvao() const { return _vao; }
		inline const vbo& getxyz() const { return _xyz; }
		inline const vbo& getuv()  const { return _uv; }
		inline const vbo& getstr() const { return _str; }
		inline const vibo& getib() const { return _ib; }

		virtual ~mesh() = 0 {};
	};
	class static_mesh : protected R3DLoader, public mesh
	{
	public:
		// from file
		void load(const char* filepath);

		// from file in memory
		void load(const char* ptr, size_t size);

		// from memory, position and indices are necessary, uvCoords and normal are optional
		void load(const float* position, const float* uvCoords, const float* normal, const unsigned int* indices, unsigned int vtsCount, unsigned int idsCount);
	};
}