/*
//@author:	zrk
//@date:    2020 / 5 / 31
//
//
//
//
//@last modified date: 2020/11/11
//+remarks:
//+ # legacy implementation #
//+ !!
//+ bcz class 'skyshader' should be taken apart from class 'skybox'
//+ !!
*/

#pragma once
#include "shader.h"
#include "vertexBuffer.h"
#include "texture2D.h"
#include "camera.h"
#include "textureCUBE.h"
#include "gbuffer.h"
namespace cat
{
	class skyshader final : private shader
	{
		int _mvp;
		skyshader(const skyshader&) { _mvp = 0; }
	public:
		skyshader();
		virtual ~skyshader();
		
		// ! fix !
		void prepareBuffer(gbuffer& gbuf) const;
		
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

		// ! fix !
		const skyshader& getShader() const { return _shd; }
	};
}