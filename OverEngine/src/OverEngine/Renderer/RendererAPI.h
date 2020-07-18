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
		virtual void Init() = 0;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(const Math::Color& color) = 0;
		virtual void SetClearDepth(float depth) = 0;
		virtual void Clear(bool clearColorBuffer, bool clearDepthBuffer) = 0;

		virtual void DrawIndexed(const VertexArray& vertexArray) = 0;

		virtual uint32_t GetMaxTextureSize() = 0;
		virtual uint32_t GetMaxTextureSlotCount() = 0;

		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}