#include "dbg.h"
#include "shader.h"

static unsigned int compileShader(const char* source, unsigned int type)
{
	CAT_CALL(unsigned int id = glCreateShader(type));
	const char* src = source;
	CAT_CALL(glShaderSource(id, 1, &src, nullptr));
	glCompileShader(id);
	// error handling:
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);	// i: integer;  v: vector;  iv: int *
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)_malloca(length * sizeof(char));	// memory in stack, not in pile
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile "
			<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!"
			<< std::endl;
		std::cout << message << std::endl;
		_freea(message);
		glDeleteShader(id);
		return 0;
	}

	return id;
}
cat::shader::shader()
{
	_program = 0;
}
cat::shader::~shader()
{
	release();
}
void cat::shader::begin()
{
	release();
	_program = glCreateProgram();
}
void cat::shader::load(const char* src, SHADER_TYPE type)
{
	CAT_ASSERT(_program != 0);
	unsigned int tp = 0;
	switch (type) {
	case SHADER_TYPE::VERTEX_SHADER:
		tp = GL_VERTEX_SHADER;
		break;
	case SHADER_TYPE::FRAGMENT_SHADER:
		tp = GL_FRAGMENT_SHADER;
		break;
	case SHADER_TYPE::COMPUTE_SHADER:
		tp = GL_COMPUTE_SHADER;
		break;
	case SHADER_TYPE::GEOMETRY_SHADER:
		tp = GL_GEOMETRY_SHADER;
		break;
	case SHADER_TYPE::TESS_CTRL_SHADER:
		tp = GL_TESS_CONTROL_SHADER;
		break;
	case SHADER_TYPE::TESS_EVAL_SHADER:
		tp = GL_TESS_EVALUATION_SHADER;
		break;
	default:
		std::cout << "warning: unrecognized shader type: " << (int)type << std::endl;
		return;
	}

	unsigned int shd = compileShader(src, tp);
	CAT_CALL(glAttachShader(_program, shd));
	CAT_CALL(glDeleteShader(shd));
}

void cat::shader::end()
{
	CAT_CALL(glLinkProgram(_program));
	CAT_CALL(glValidateProgram(_program));
}

void cat::shader::bind() const
{
	// no CAT_CALL
	glUseProgram(_program);
}

void cat::shader::unbind() const
{
	// no CAT_CALL
	glUseProgram(0);
}

void cat::shader::release()
{
	if (_program) {
		CAT_CALL(glDeleteProgram(_program));
		_program = 0;
	}
}

int cat::shader::getlocation(const char* name) const
{
	int loc;
	CAT_CALL(loc = glGetUniformLocation(_program, name));
	if (loc == -1) {
		std::cout << "warning: uniform " << name << " doesn't exist!" << std::endl;
	}
	return loc;
}
void cat::shader::setint(int loc, const int& i) const
{
	bind();
	if (loc != -1)
		(glUniform1i(loc, i));
}
void cat::shader::setuint(int loc, const unsigned int& i) const
{
	bind();
	if (loc != -1)
		CAT_CALL(glUniform1ui(loc, i));
}
void cat::shader::setfloat(int loc, const float& fl) const
{
	bind();
	if (loc != -1)
		(glUniform1f(loc, fl));
}

void cat::shader::setvec2(int loc, const glm::vec2& v) const
{
	bind();
	if (loc != -1)
		(glUniform2f(loc, v.x, v.y));
}

void cat::shader::setvec3(int loc, const glm::vec3& v) const
{
	bind();
	if (loc != -1)
		(glUniform3f(loc, v.x, v.y, v.z));
}

void cat::shader::setvec4(int loc, const glm::vec4& v) const
{
	bind();
	if (loc != -1)
		(glUniform4f(loc, v.x, v.y, v.z, v.w));
}

void cat::shader::setmat4(int loc, const glm::mat4& mt) const
{
	bind();
	if (loc != -1)
		(glUniformMatrix4fv(loc, 1, GL_FALSE, &mt[0][0]));
}
