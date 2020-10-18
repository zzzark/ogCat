/*
//@author:	zark
//@time:	2020 / 5 / 30
//example:

shader shd;
shd.begin();
shd.load(shader_source, type);
...
shd.end();

shader::uniform mvp = shd["u_MVP"];		// getUniformLocation
mvp = glm::mat4(1.0f);					// glUniformXXXX

*/

#pragma once
#include "glm/glm.hpp"
#include <string>

namespace cat
{

	class shader
	{
	private:
		unsigned int _program;
		shader(shader&) { _program = 0; }
	protected:

	public:
		enum class SHADER_TYPE {
			VERTEX_SHADER,
			FRAGMENT_SHADER,

			TESS_CTRL_SHADER,
			TESS_EVAL_SHADER,

			GEOMETRY_SHADER,

			COMPUTE_SHADER,
		};
		class uniform {
			int _loc;
			const shader* _shd;
			uniform(int loc, const shader* shd) { _loc = loc; _shd = shd; }
		public:
			friend shader;
			inline void operator=(const int& i)				{ _shd->setint(_loc, i); }
			inline void operator=(const unsigned int& ui)	{ _shd->setuint(_loc, ui); }
			inline void operator=(const float& f)			{ _shd->setfloat(_loc, f); }
			inline void operator=(const glm::vec2& v)		{ _shd->setvec2(_loc, v); }
			inline void operator=(const glm::vec3& v)		{ _shd->setvec3(_loc, v); }
			inline void operator=(const glm::vec4& v)		{ _shd->setvec4(_loc, v); }
			inline void operator=(const glm::mat4& m)		{ _shd->setmat4(_loc, m); }
		};
		shader();
		virtual ~shader();
		void begin();
		void load(const char* src, SHADER_TYPE type);
		void end();

		void bind() const;
		void unbind() const;

		void release();

		int getlocation(const char* name) const;

		void setuint(int loc, const unsigned int& i) const;
		void setint(int loc, const int& i) const;
		void setfloat(int loc, const float& fl) const;
		void setvec2(int loc, const glm::vec2& v) const;
		void setvec3(int loc, const glm::vec3& v) const;
		void setvec4(int loc, const glm::vec4& v) const;
		void setmat4(int loc, const glm::mat4& mt) const;

		unsigned int getProgram() const { return _program; }
		inline uniform operator[] (const char* name) const {
			return uniform(getlocation(name), this);
		}
		
	};
}