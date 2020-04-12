#include <OverEngine.h>

#include "imgui/imgui.h"

class ExampleLayer : public OverEngine::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		//OE_INFO("ExampleLayer::Update");
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("I am rendering from client!");
		ImGui::Text("Dock other windows down here by dragging from titlebar");
		ImGui::DockSpace(10);
		ImGui::End();
	}

	void OnEvent(OverEngine::Event& event) override
	{
		//OE_TRACE("{0}", event);
	}

};

class OverPlayer : public OverEngine::Application
{
public:
	OverPlayer()
	{
		PushLayer(new ExampleLayer());
	}

	~OverPlayer()
	{

	}
};

OverEngine::Application* OverEngine::CreateApplication()
{
	return new OverPlayer();
}