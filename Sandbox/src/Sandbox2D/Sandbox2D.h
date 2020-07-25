#if 0
#pragma once

#include <OverEngine.h>

using namespace OverEngine;

class Sandbox2D: public Layer
{
public:
	Sandbox2D();

	void OnAttach() override {};
	void OnUpdate(TimeStep DeltaTime) override;
	void OnImGuiRender() override;
	void OnEvent(Event& event) override;

	bool OnWindowResizeEvent(WindowResizeEvent& event);
	bool OnMouseScrolledEvent(MouseScrolledEvent& event);
private:
	OrthographicCamera m_Camera;

	float m_CameraSpeed = 1.0f;
	float m_CameraRotationDirection = 0.0f;
	Vector2 m_CameraMovementDirection;

	Ref<Texture2D> m_CheckerBoardTexture;
	Ref<Texture2D> m_OELogoTexture;
	Ref<Texture2D> m_SpriteSheet;
	Ref<Texture2D> m_Sprite;
};
#endif