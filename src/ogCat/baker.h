#pragma once
#include "feedback.h"
#include <vector>
namespace cat
{
	class oven
	{
	protected:
		fbShader _shd;
		std::vector<const vbo*> _list;
	public:
		void create(const char* vs, const char* gs, const char* vars[], unsigned int count);
		void begin();
		void prepare(const vbo& dest, const vbo& src);
		void bake();
		void end();
	};
}