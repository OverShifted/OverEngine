#define OE_CLIENT_INCLUDE_ENTRY_POINT
#include <OverEngine.h>

#include "SandboxLayer.h"
#include "Sandbox2D.h"

class SandboxApp : public OverEngine::Application
{
public:
	SandboxApp()
	{
		//PushLayer(new SandboxLayer());
		PushLayer(new Sandbox2D());
		m_ImGuiEnabled = true;
	}

	~SandboxApp()
	{

	}
};

OverEngine::Application* OverEngine::CreateApplication()
{
	return new SandboxApp();
}