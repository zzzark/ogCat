#pragma once
#include "vertexBuffer.h"
#include "shader.h"

namespace cat
{
	class fbShader : public shader
	{
		void end() {}
	public:
		void end(const char* vars[], unsigned int count);
		void end(const char* var);
	};
	class feedback
	{
	public:
		enum class FB_TYPE
		{
			FB_POINTS,
			FB_LINES,
			FB_TRIANGLES,
		};
		static void begin(FB_TYPE type);
		static void end();
		static void bind(const cat::vbo& obj, unsigned int slot);
		static void unbind(const cat::vbo& obj);
	};
}