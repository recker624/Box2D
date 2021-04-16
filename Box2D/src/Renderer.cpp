#include"Renderer.h"

#include<iostream>

void GLClearError() {
	//keep looping until we retrieve all the errors
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* func, const char* file, int line) {
	while (GLenum error = glGetError()) {
		std::cout << std::hex <<"[OpenGL error] (" << error << ") " << func << " " << file << " : " << line << std::endl;
		return true;
	}
	return false;
}

void Renderer::Clear() const
{
	GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::Draw(Shader &shader, VertexArray &va)
{
	va.Bind();
	shader.Bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
}
