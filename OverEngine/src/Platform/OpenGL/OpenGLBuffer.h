#pragma once

#include "OverEngine/Renderer/Buffer.h"

namespace OverEngine
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer();
		OpenGLVertexBuffer(const void* vertices, uint32_t size, bool staticDraw);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void BufferData(const void* vertices, uint32_t size, bool staticDraw = true) const override;
		virtual void BufferSubData(const void* vertices, uint32_t size, uint32_t offset = 0) const override;
		virtual void AllocateStorage(uint32_t size) const override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
	private:
		uint32_t m_RendererID = 0;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer();
		OpenGLIndexBuffer(const uint32_t* indices, uint32_t count, bool staticDraw);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void BufferData(const uint32_t* indices, uint32_t count, bool staticDraw = true) const override;
		virtual void BufferSubData(const uint32_t* indices, uint32_t count, uint32_t offset = 0) const override;
		virtual void AllocateStorage(uint32_t count) const override;

		virtual uint32_t GetCount() const { return m_Count; }
	private:
		uint32_t m_RendererID = 0;
		mutable uint32_t m_Count;
	};
}
