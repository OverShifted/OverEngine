#pragma once

#include "VertexArray.h"

namespace OverEngine
{
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};
	public:
		virtual void SetClearColor(const Math::Color& color) = 0;
		virtual void SetClearDepth(float depth) = 0;
		virtual void Clear(bool clearColorBuffer, bool clearDepthBuffer) = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) = 0;

		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}