#pragma once

#include <GL/glew.h>
#include "Utils/Debug.h"

#define LogOpenGLErrors() LogOGLErrors(__FILE__, __LINE__)

inline void LogOGLErrors(const char *file, int line)
{
	while (GLenum glErr = glGetError())
		Esteem::Debug::LogError("OpenGL Error: ", (const char*)gluErrorString(glErr), " (", glErr, + ") recorded at: ", file, " on line: ", line);
}