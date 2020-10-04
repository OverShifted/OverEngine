#pragma once

#include "VertexArray.h"
#include "ClearFlags.h"

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
		virtual void Clear(const ClearFlags& flags) = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		virtual uint32_t GetMaxTextureSize() = 0;
		virtual uint32_t GetMaxTextureSlotCount() = 0;

		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}
