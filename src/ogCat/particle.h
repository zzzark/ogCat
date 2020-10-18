/*
@author:	zrk
@date:		2020/7/9
*/
#pragma once
#include <vector>
#include "texture2D.h"
#include "feedback.h"
#include "camera.h"
namespace cat
{
	class sparks
	{
		shader _shd;
		texture2D _tex;
		vao _vao;
		vbo _pos;
		vbo _int;
		unsigned int _count = 0;
		int _loc_mvp = -1;
		int _loc_right = -1;
		int _loc_up    = -1;
	public:
		void setSize(float sz);
		void setIntensity(float v);
		void begin(const char* filepath, unsigned int count);
		void addPos(const void* data, unsigned int sizeInBytes);
		void addInt(const void* data, unsigned int sizeInBytes);
		void end();
		const vbo& getPos() const { return _pos; }
		const vbo& getInt() const { return _int; }
		virtual void draw(const camera& cam);
		virtual ~sparks();
	};
	class weed
	{
		texture2D _tex;
		vao _vao;
		vbo _xyz;
		vbo _uv;
		vbo _str;
		vibo _ibo;
	public:
		void create(const char* filepath, unsigned int count, float scale, const float* data);
		virtual void draw(const camera& cam);
		virtual ~weed();
	};
}