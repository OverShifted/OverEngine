#if 0
#pragma once

#include <OverEngine.h>

class SandboxLayer : public OverEngine::Layer
{
public:
	SandboxLayer();

	void OnAttach() override;
	void OnUpdate(OverEngine::TimeStep deltaTime) override;
	void OnImGuiRender() override;
	void OnEvent(OverEngine::Event& event) override;

	bool OnWindowResizeEvent(OverEngine::WindowResizeEvent& event);
	bool OnMouseScrolledEvent(OverEngine::MouseScrolledEvent& event);
private:
	OverEngine::String vendorInfo, rendererInfo, versionInfo;

	OverEngine::Ref<OverEngine::Shader> m_Shader;
	OverEngine::Ref<OverEngine::VertexArray> m_VertexArray;

	OverEngine::Ref<OverEngine::Texture2D> m_CheckerBoardTexture, m_OELogoTexture;
	OverEngine::Ref<OverEngine::VertexArray> m_SquareVA;

	OverEngine::Transform m_TriangleTransform;
	OverEngine::Transform m_SquareTransform;

	OverEngine::OrthographicCamera m_Camera;

	float m_CameraSpeed = 1.0f;
	float m_CameraRotationDirection = 0.0f;
	OverEngine::Vector2 m_CameraMovementDirection;
};
#endif
