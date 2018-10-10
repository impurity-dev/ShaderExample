#include "Renderer.h"
#include <iostream>

// Clear all existing errors
void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

// Log if there is an error in OpenGL
bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}