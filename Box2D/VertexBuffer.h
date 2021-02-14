#pragma once

class VertexBuffer {
	//the ID of the buffer object
private:
	unsigned int m_RendererID;

public:
	VertexBuffer(const float* data, unsigned int size);
	~VertexBuffer();

	//for unbinding and unbinding the buffer objects
	void Bind() const;
	void UnBind() const;
};