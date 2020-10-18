/*
//@author:	zrk
//@time:	2020 / 6/ 2
*/
#pragma once
#include "vertexBuffer.h"
#include "renderer.h"
#include "frameBuffer.h"

namespace cat
{
	class frameEffect
	{
		vao _vao;
		vbo _vbo;
		vibo _ibo;
		frameShader _shd;
	public:
		virtual ~frameEffect();

		// remarks:
		//  frameShader.h: the output "vec2 v_uv" in vs means "texture coords"
		//  e.g.
		//			#version 450 core       
		//			in vec2 v_uv;           
		//			uniform sampler2D u_tex;
		//			color = texture2D(u_tex, v_uv).xyz;
		//
		void create(const char* fs);

		// clear the fbuffer and then do rendering
		//
		void render(float r, float g, float b, float a, bool swap = true) const;
		
		// do rendering
		// 
		void renderPURE() const;


		void release();
		inline const frameShader& shader() const { return _shd; }
	};
}