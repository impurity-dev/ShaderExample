#pragma once

#include <GL/glew.h>

// If our value is false, lets break the debugger
#define ASSERT(x) if(!(x)) __debugbreak();
// Will Clear our errors then assert if there are new errors and break debugger if there are
// '#' will turn x into a string. This will get us our function name
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__));

// Clear all existing errors
void GLClearError();
// Log if there is an error in OpenGL
bool GLLogCall(const char* function, const char* file, int line);
