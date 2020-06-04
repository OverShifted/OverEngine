#pragma once

#include <OverEngine.h>

class GameLayer : public OverEngine::Layer
{
public:
	GameLayer();

	void OnAttach() override;
	void OnUpdate() override;
	void OnImGuiRender() override;
	void OnEvent(OverEngine::Event& event) override;
private:
	OverEngine::String vendorInfo, rendererInfo, versionInfo;

	OverEngine::Ref<OverEngine::Renderer::Shader> m_Shader;
	OverEngine::Ref<OverEngine::Renderer::VertexArray> m_VertexArray;

	OverEngine::Ref<OverEngine::Renderer::Shader> m_BlueShader;
	OverEngine::Ref<OverEngine::Renderer::VertexArray> m_SquareVA;
};