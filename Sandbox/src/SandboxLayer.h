#pragma once

#include <OverEngine.h>

class SandboxLayer : public OverEngine::Layer
{
public:
	SandboxLayer();

	void OnAttach() override;
	void OnUpdate(OverEngine::TimeStep DeltaTime) override;
	void OnImGuiRender() override;
	void OnEvent(OverEngine::Event& event) override;

	bool OnWindowResizeEvent(OverEngine::WindowResizeEvent& event);
	bool OnMouseScrolledEvent(OverEngine::MouseScrolledEvent& event);
private:
	OverEngine::String vendorInfo, rendererInfo, versionInfo;

	OverEngine::Ref<OverEngine::Shader> m_Shader;
	OverEngine::Ref<OverEngine::VertexArray> m_VertexArray;

	OverEngine::Ref<OverEngine::Shader> m_FlatColorFragmentShader, m_TextureShader;
	OverEngine::Ref<OverEngine::Texture2D> m_BreadTexture, m_ChernoLogoTexture;
	OverEngine::Ref<OverEngine::VertexArray> m_SquareVA;

	OverEngine::Transform m_TriangleTransform;
	OverEngine::Transform m_SquareTransform;

	OverEngine::Ref<OverEngine::Camera> m_Camera;

	float m_CameraSpeed = 2.4f;
};