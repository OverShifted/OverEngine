#pragma once

#include <OverEngine.h>
#include <OverEngine/Scripting/Wren.h>

using namespace OverEngine;

class SandboxECS : public Layer
{
public:
	SandboxECS();

	void OnUpdate(TimeStep deltaTime) override;
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

	Ref<Wren> m_VM;
};
