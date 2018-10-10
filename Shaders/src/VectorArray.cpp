#include "VertexArray.h"
#include "Renderer.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer & vb, const VertexBufferLayout & layout)
{
	// Bind this Vertex Array
	Bind();
	// Bind the Vertext Buffer
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++) 
	{
		// Grab the current Vertex Buffer Layout
		const auto& element = elements[i];
		// Enable the vertex attribute
		GLCall(glEnableVertexAttribArray(0));
		// Define the structure of our buffer input
		// First attribute, 2 components define this attribute, they are floats, not normalized, 2 float values define the size, next attribute offset
		// This call will link index 0 of this vertex array will be bound to the currently bound array buffer (i.e. buffer^)
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset));
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
