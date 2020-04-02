#include <OverEngine.h>

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
		PushLayer(new OverEngine::ImGuiLayer());
		OE_INFO("Client Application Init");
	}

	~OverPlayer()
	{

	}
};

OverEngine::Application* OverEngine::CreateApplication()
{
	return new OverPlayer();
}