#define OE_CLIENT_INCLUDE_ENTRY_POINT
#include <OverEngine.h>

#include "SandboxLayer/SandboxLayer.h"
#include "Sandbox2D/Sandbox2D.h"
#include "SandboxECS/SandboxECS.h"

static ApplicationProps GenApplicationProps()
{
	ApplicationProps props;
	props.MainWindowProps.Title = "OverEngine's Sandbox Demo";
	props.MainWindowProps.Width = 1600;
	props.MainWindowProps.Height = 900;
	return props;
}

class SandboxApp : public OverEngine::Application
{
public:
	SandboxApp()
		: Application(GenApplicationProps())
	{
		//PushLayer(new SandboxLayer());
		//PushLayer(new Sandbox2D());
		PushLayer(new SandboxECS());
		m_ImGuiEnabled = true;
	}

	~SandboxApp()
	{

	}
};

OverEngine::Application* OverEngine::CreateApplication(int argc, char** argv)
{
	return new SandboxApp();
}
