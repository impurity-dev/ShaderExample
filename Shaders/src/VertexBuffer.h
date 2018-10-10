#pragma once

class VertexBuffer
{
private:
	unsigned int m_Renderer_Id; // ID for the renderer that we use to fetch the object from the renderer
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};