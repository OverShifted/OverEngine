#define OE_CLIENT_INCLUDE_ENTRY_POINT
#include <OverEngine.h>

#include "SandboxLayer.h"

class SandboxApp : public OverEngine::Application
{
public:
	SandboxApp()
	{
		PushLayer(new SandboxLayer());
	}

	~SandboxApp()
	{

	}
};

OverEngine::Application* OverEngine::CreateApplication()
{
	return new SandboxApp();
}