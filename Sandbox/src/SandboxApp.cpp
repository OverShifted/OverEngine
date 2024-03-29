#define OE_CLIENT_INCLUDE_ENTRY_POINT
#include <OverEngine.h>

#include "SandboxLayer/SandboxLayer.h"
#include "Sandbox2D/Sandbox2D.h"
#include "SandboxECS/SandboxECS.h"
#include "StringSim/StringSim.h"
#include "NeuralNetwork/NeuralNetwork.h"

static ApplicationProps GenApplicationProps()
{
	ApplicationProps props;
	props.MainWindowProps.Title = "Sandbox";
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
		// PushLayer(new StringSim());
		// PushLayer(new NeuralNetwork());
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
