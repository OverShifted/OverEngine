#include "pcheader.h"
#include "OpenGLBuffer.h"

#include <glad/gl.h>

namespace OverEngine
{
	/////////////////////////////////////////////////////////////////////////////
	// VertexBuffer /////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	OpenGLVertexBuffer::OpenGLVertexBuffer(const float* vertices, uint32_t size, bool staticDraw)
	{
		glCreateBuffers(1, &m_RendererID);
		BufferData(vertices, size, staticDraw);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer()
	{
		glCreateBuffers(1, &m_RendererID);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::BufferData(const float* vertices, uint32_t size, bool staticDraw /*= true*/) const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, staticDraw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}

	void OpenGLVertexBuffer::BufferSubData(const float* vertices, uint32_t size, uint32_t offset /*= 0*/) const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, vertices);
	}

	void OpenGLVertexBuffer::AllocateStorage(uint32_t size) const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	/////////////////////////////////////////////////////////////////////////////
	// IndexBuffer //////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* indices, uint32_t count, bool staticDraw)
		: m_Count(count)
	{
		glCreateBuffers(1, &m_RendererID);
		BufferData(indices, count, staticDraw);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer()
		: m_Count(0)
	{
		glCreateBuffers(1, &m_RendererID);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGLIndexBuffer::BufferData(const uint32_t* indices, uint32_t count, bool staticDraw /*= true*/) const
	{
		m_Count = count;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, staticDraw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}

	void OpenGLIndexBuffer::BufferSubData(const uint32_t* indices, uint32_t count, uint32_t offset /*= 0*/) const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset * sizeof(uint32_t), count * sizeof(uint32_t), indices);
	}

	void OpenGLIndexBuffer::AllocateStorage(uint32_t count) const
	{
		if (count <= m_Count)
			return;

		m_Count = count;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
	}
}
