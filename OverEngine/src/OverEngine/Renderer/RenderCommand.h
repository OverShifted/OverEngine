#pragma once

#include "RendererAPI.h"

namespace OverEngine
{
	class RenderCommand
	{
	public:
		inline static void Init()
		{
			s_RendererAPI->Init();

			s_MaxTextureSize = s_RendererAPI->GetMaxTextureSize();
			s_MaxTextureSlotCount = s_RendererAPI->GetMaxTextureSlotCount();
		}

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		inline static void SetClearColor(const Math::Color& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		inline static void SetClearDepth(float depth)
		{
			s_RendererAPI->SetClearDepth(depth);
		}

		inline static void Clear(const ClearFlags& flags = ClearFlags_ClearColor | ClearFlags_ClearDepth)
		{
			s_RendererAPI->Clear(flags);
		}

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0, DrawType drawType = DrawType::Triangles)
		{
			s_RendererAPI->DrawIndexed(vertexArray, indexCount, drawType);
		}

		inline static uint32_t GetMaxTextureSize()
		{
			return s_MaxTextureSize;
		}

		inline static uint32_t GetMaxTextureSlotCount()
		{
			return s_MaxTextureSlotCount;
		}
	private:
		static RendererAPI* s_RendererAPI;

		static uint32_t s_MaxTextureSize;
		static uint32_t s_MaxTextureSlotCount;
	};
}
