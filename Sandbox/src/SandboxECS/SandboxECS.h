#pragma once

#include <OverEngine.h>
#include <sol/sol.hpp>

using namespace OverEngine;

class SandboxECS : public Layer
{
public:
	SandboxECS();

	void OnAttach() override {};
	void OnUpdate(TimeStep DeltaTime) override;
	void OnImGuiRender() override;
	void OnEvent(Event& event) override;

	bool OnMouseScrolledEvent(MouseScrolledEvent& event);
	bool OnKeyPressedEvent(KeyPressedEvent& event);
private:

	Ref<Texture2D> m_SpriteSheet;
	Ref<Texture2D> m_Sprite;
	Ref<Texture2D> m_ObstacleSprite;

	Ref<Scene> m_Scene;
	Entity m_Player;
	Entity m_MainCamera;

	ParticleSystem2D m_ParticleSystem;
};
