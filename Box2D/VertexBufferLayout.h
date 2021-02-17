#pragma once
#include<vector>
#include "Renderer.h"

/*
* For the struct below:
	type		-	since the OpenGL types have a hex value associated with them 
					that's why here the type of data is an unsigned int
	count		-	The number of elements in one vertex attribute
	normalized	-	whether the vertex data should be normalized or not
	GetSizeOfType() - returns the size of each type data that our vertex buffer is storing
*/
		
struct VertexBufferElement
{
	unsigned int type;		
	unsigned int count;		
	unsigned int normalized;
	
	static unsigned int GetSizeOfType(unsigned int type) 
	{
		switch (type) {
			case GL_FLOAT :			return 4;
			case GL_UNSIGNED_INT :	return 4;
			case GL_UNSIGNED_BYTE : return 1;
		}
		ASSERT(false);
		return 0;
	}
};

class VertexBufferLayout 
{
private:
	//a list of all the vertex buffer elements in the currently bound vertex array object
	std::vector<VertexBufferElement> m_Element;
	unsigned int m_stride;
public:
	VertexBufferLayout() : m_stride(0) {}

	template<typename t>
	void Push(unsigned int count)
	{
		static_assert(false);
	}

	template<>
	void Push<float>(unsigned int count)
	{
		m_Element.push_back({GL_FLOAT, count, GL_FALSE});
		m_stride += VertexBufferElement::GetSizeOfType(GL_FLOAT) * count;
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		m_Element.push_back({ GL_UNSIGNED_INT, count, GL_FALSE});
		m_stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT) * count;
	}

	template<>
	void Push<unsigned char>(unsigned int count) 
	{
		m_Element.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE});
		m_stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE) * count;
	}

	//we need to be able to get the stride of the data in the buffer and also return the elements
	inline const std::vector<VertexBufferElement>& GetElements() const { return m_Element; }
	inline unsigned int GetStride() const { return m_stride; }
};