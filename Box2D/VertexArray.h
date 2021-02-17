#pragma once

#include"VertexBuffer.h"
#include"VertexBufferLayout.h"

class VertexArray {
private:
	unsigned int m_RendererID;
public:
	VertexArray();
	~VertexArray();

	//Since a vertex array object can store multiple buffers inside it so we
	//create a separate function to be able to add different vertex buffers to it
	void AddBuffers(const VertexBuffer& vb, const VertexBufferLayout& layout);

	//to bind and unbind the vertex array
	void Bind() const;
	void Unbind() const;
};