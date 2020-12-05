/*
@author:	zrk
@date:		2020/7/9
//@last modified date: 2020/11/11
//+remarks:
//+ # legacy implementation #
*/
#pragma once
#include <vector>
#include "gbuffer.h"
#include "texture2D.h"
#include "feedback.h"
#include "camera.h"
namespace cat
{
	class sparks
	{
	protected:
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
		void setSize(float sz) const;
		void setIntensity(float v) const;
		void begin(const char* filepath, unsigned int count);
		void addPos(const void* data, unsigned int sizeInBytes);
		void addInt(const void* data, unsigned int sizeInBytes);
		void end();
		const vbo& getPos() const { return _pos; }
		const vbo& getInt() const { return _int; }
		void render(const gbuffer& gbuf, const camera& cam);
		unsigned int getCount() const { return _count; }
		virtual ~sparks();
	};
	class sparksController
	{
	protected:
		struct vbo_layout {
			vbo* _ptr = nullptr;
			int _count = 0;
			cat::VERTEX_TYPE _type = cat::VERTEX_TYPE::VT_FLOAT;
		};
		vao _vao;
		std::vector<vbo_layout> _vbos;
		fbShader _shd;
	public:
		// note:
		//	location 0: vec3  pos
		//	location 1: float intensity
		//  out 0: MUST BE vec3  [pos]
		//	out 1: MUST BE float [itn]
		//		
		void begin(const char* vs, const char* var0 = "pos", const char* var1 = "itn");
		void addAttribute(const void* data, unsigned int sizeInBytes, int count, cat::VERTEX_TYPE type);
		void end();
		void update(const sparks& spk);
		virtual ~sparksController();
		const fbShader& shader() const { return _shd; }
	};
	class weed
	{
	protected:
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