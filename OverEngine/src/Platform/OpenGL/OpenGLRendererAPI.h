#pragma once

#include "OverEngine/Renderer/RendererAPI.h"

namespace OverEngine
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void SetClearColor(const Math::Color& color) override;
		virtual void SetClearDepth(float depth) override;
		virtual void Clear(const ClearFlags& flags) override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0, DrawType drawType = DrawType::Triangles) override;

		virtual uint32_t GetMaxTextureSize() override;
		virtual uint32_t GetMaxTextureSlotCount() override;

		virtual bool IsDepthTestingEnabled() override;
		virtual void DisableDepthTesting() override;
		virtual void EnableDepthTesting() override;
	};
}
