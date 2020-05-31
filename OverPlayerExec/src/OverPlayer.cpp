#define OE_CLIENT_INCLUDE_ENTRY_POINT
#include <OverEngine.h>

#include "GameLayer.h"

class OverPlayer : public OverEngine::Application
{
public:
	OverPlayer()
	{
		PushLayer(new GameLayer());
	}

	~OverPlayer()
	{

	}
};

OverEngine::Application* OverEngine::CreateApplication()
{
	return new OverPlayer();
}