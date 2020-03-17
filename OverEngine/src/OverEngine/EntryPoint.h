#pragma once
#ifdef OE_PLATFORM_WINDOWS

#include <iostream>
extern  OverEngine::Application* OverEngine::CreateApplication();

int main(int argc, char** argv)
{
	auto app = OverEngine::CreateApplication();
	std::cout << "salam";
	app->Run();
	delete app;
}
#endif // OE_PLATFORM_WINDOWS
