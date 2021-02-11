#include<iostream>

#include"VertexBuffer.h"
#include"Renderer.h"

VertexBuffer::VertexBuffer(const float* data, unsigned int size, unsigned int &vao) {
	GLCall(glGenBuffers(1, &m_RendererID));

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer() {
	//GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::UnBind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}