#pragma once

#include "pcheader.h"

#include "Init.h"

extern OverEngine::Application* OverEngine::CreateApplication();

int OverMain(int argc, char** argv) // Platform agnostic entry point
{
	OverEngine::Initialize();

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

#ifdef OE_PLATFORM_UNIX_BASE
int main(int argc, char** argv)
{
	return OverMain(argc, argv);
}
#endif // OE_PLATFORM_UNIX_BASE