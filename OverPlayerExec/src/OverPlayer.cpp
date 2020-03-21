#include <OverEngine.h>

class OverPlayer : public OverEngine::Application
{
public:
	OverPlayer()
	{

	}

	~OverPlayer()
	{

	}
};

OverEngine::Application* OverEngine::CreateApplication()
{
	return new OverPlayer();
}