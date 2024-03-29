#include<iostream>

#include"VertexBuffer.h"
#include"Renderer.h"

//create the vertex buffer object
VertexBuffer::VertexBuffer(const float* data, unsigned int size) {
	GLCall(glGenBuffers(1, &m_RendererID));	
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer() {
	GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::UnBind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}