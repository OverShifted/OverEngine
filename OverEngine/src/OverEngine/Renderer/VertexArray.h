#pragma once

#include "OverEngine/Core/Core.h"

#include "OverEngine/Renderer/Buffer.h"

namespace OverEngine
{
	class VertexArray
	{
	public:
		static Ref<VertexArray> Create();

		virtual ~VertexArray() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

		virtual const Vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;
	};
}
