#pragma once
#include"pcheader.h"

extern OverEngine::Application* OverEngine::CreateApplication();

int OverMain(int argc, char** argv) // Platform agnostic entry point
{
	OverEngine::Log::Init();
	OE_CORE_INFO("OverEngine v0.0");

	auto app = OverEngine::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#ifdef OE_PLATFORM_WINDOWS
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)

int main(int argc, char** argv)
{
	return OverMain(argc, argv);
}
#endif // OE_PLATFORM_WINDOWS