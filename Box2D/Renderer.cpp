#include"Renderer.h"
#include<iostream>

void GLClearError() {
	//keep looping until we retrieve all the errors
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* func, const char* file, int line) {
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL error] (" << error << ") " << func << " " << file << " : " << line << std::endl;
		return true;
	}
	return false;
}