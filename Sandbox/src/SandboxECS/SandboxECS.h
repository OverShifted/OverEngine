#pragma once

#include <OverEngine.h>
#include <sol/sol.hpp>

using namespace OverEngine;

class SandboxECS: public Layer
{
public:
	SandboxECS();

	void OnAttach() override {};
	void OnUpdate(TimeStep DeltaTime) override;
	void OnImGuiRender() override;
	void OnEvent(Event& event) override;

	bool OnMouseScrolledEvent(MouseScrolledEvent& event);
private:

	float m_CameraSpeed = 1.0f;
	float m_CameraRotationDirection = 0.0f;
	Vector2 m_CameraMovementDirection;

	Ref<Texture2D> m_CheckerBoardTexture;
	Ref<Texture2D> m_OELogoTexture;

	Ref<Texture2D> m_SpriteSheet;
	Ref<Texture2D> m_Sprite;
	Ref<Texture2D> m_ObstacleSprite;

	Ref<Scene> m_Scene;
	Entity m_Player;
	Entity m_MainCamera;
	TransformComponent* m_MainCameraTransform;
	SceneCamera* m_MainCameraCameraHandle;
};
