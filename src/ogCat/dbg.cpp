#include "dbg.h"

void cat::ClearError()
{
	while (glGetError() != GL_NO_ERROR);
}
bool cat::ErrorLog(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] (" << std::hex << error << "):" << function << "  " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

