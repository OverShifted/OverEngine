#include <OverEngine.h>

#include "imgui/imgui.h"

#include "ExampleLayer.h"

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