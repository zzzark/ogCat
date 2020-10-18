/*
//@author:	zark
//@time:	2020 / 5 / 31
//
//
//remarks:
//two ways to create cube texture:
// #1
textureCBUE cb;
cb.create("right", "left", "up", "down", "front", "back");
cb.active();
drawsomething();

// #2
textureCUBE cb;
cb.begin();
for(int i = 0; i < 6; i++)
	cb.createFace(i, w, h, TT_RGB, TDT_BYTE, pointer);
		// i = 0: right
		// i = 1: left
		// i = 2: up
		// i = 3: down
		// i = 4: front
		// i = 5: back
cb.end();
cb.active();
drawsomething();

*/

#pragma once
#include "shader.h"
#include "vertexBuffer.h"
#include "texture2D.h"
#include "camera.h"
#include "textureCUBE.h"
namespace cat
{
	class skyshader final : private shader
	{
		int _mvp;
		skyshader(const skyshader&) { _mvp = 0; }
	public:
		skyshader();
		virtual ~skyshader();
		inline void bind() const { shader::bind(); }
		inline void unbind() const { shader::unbind(); }
		void setmat4(const glm::mat4& m) const;
		inline uniform operator[] (const char* name) const {
			return shader::operator[](name);
		}
	};
	class skybox final
	{
		vao _ao;
		vbo _bo;
		vibo _ib;
		skyshader _shd;
		textureCUBE _tex;
		glm::mat4 _orig;
	public:
		skybox();
		~skybox();
		void setIntensity(float v);
		inline void createTEX(const char* f0, const char* f1, const char* f2, const char* f3, const char* f4, const char* f5) {
			_tex.create(f0, f1, f2, f3, f4, f5);
		}
		inline void beginTEX() {
			_tex.begin();
		}
		inline void createFaceTEX(unsigned int index, unsigned int width, unsigned int height, TEXTURE_TYPE type, TEXTURE_DATA_TYPE dtype, const void* data) {
			_tex.createFace(index, width, height, type, dtype, data);
		}
		inline void endTEX() {
			_tex.end();
		}
		inline glm::mat4& getTransform() {
			return _orig;
		}
		inline const textureCUBE& getTex() const { return _tex; }
		void draw(const camera& cam);
		void draw(const glm::mat4& cmb);
	};
}