#include <OverEngine.h>
#include <iostream>

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

int main()
{
	OverPlayer* player = new OverPlayer();
	player->Run();
	delete player;
	//std::cout << "salam";
}