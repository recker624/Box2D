#include"VertexArray.h"
#include"VertexBufferLayout.h"
#include"Renderer.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_RendererID);
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

//adds buffer objects to the currently bound vertex array object
//for more info refer to this : https://learnopengl.com/img/getting-started/vertex_array_objects.png
void VertexArray::AddBuffers(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();		//Bind the vertex array object
	vb.Bind();	//Bind the buffer object so that it's added to the vertex array

	//now set up the layout for the buffer object
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;

	//specify the format for each vertex buffer element
	for (unsigned int i = 0; i < elements.size(); i++) 
	{
		const auto& element = elements[i];
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized,
												layout.GetStride(), (const void*) offset));
		GLCall(glEnableVertexAttribArray(i));
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}