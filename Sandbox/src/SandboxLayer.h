#pragma once

#include <OverEngine.h>

#include <OverEngine/Renderer/Camera.h>

class SandboxLayer : public OverEngine::Layer
{
public:
	SandboxLayer();

	void OnAttach() override;
	void OnUpdate() override;
	void OnImGuiRender() override;
	void OnEvent(OverEngine::Event& event) override;

	bool OnWindowResizeEvent(OverEngine::WindowResizeEvent& event);
	bool OnMouseScrolledEvent(OverEngine::MouseScrolledEvent& event);
private:
	OverEngine::String vendorInfo, rendererInfo, versionInfo;

	OverEngine::Ref<OverEngine::Renderer::Shader> m_Shader, m_BlueShader;
	OverEngine::Ref<OverEngine::Renderer::VertexArray> m_VertexArray, m_SquareVA;

	OverEngine::Ref<OverEngine::Renderer::Camera> m_Camera;

	float m_CameraSpeed = 0.04f;
};