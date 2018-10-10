#pragma once

class IndexBuffer
{
private:
	unsigned int m_Renderer_Id; // ID for the renderer that we use to fetch the object from the renderer
	unsigned int m_Count; // How many indices does this buffer have
public:
	IndexBuffer(const unsigned int* data, unsigned int size);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetCount() const { return m_Count; }
};