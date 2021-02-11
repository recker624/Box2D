#pragma once

class IndexBuffer {
private:
	//the ID of the buffer object
	unsigned int m_RendererID;
	//the number of indices
	unsigned int m_Count;

public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	//for unbinding and unbinding the buffer objects
	void Bind() const;
	void UnBind() const;

	inline unsigned int GetCount() const { return m_Count;  }
};