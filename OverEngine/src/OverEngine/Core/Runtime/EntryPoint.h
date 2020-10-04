#pragma once

#include "pcheader.h"

#include <fstream>

extern OverEngine::Application* OverEngine::CreateApplication(int argc, char** argv);

int OverMain(int argc, char** argv) // Platform agnostic entry point
{
	OE_PROFILE_BEGIN_SESSION("StartUp", "OverEngineProfile-StartUp.json");
	auto app = OverEngine::CreateApplication(argc, argv);
	OE_PROFILE_END_SESSION();

	OE_PROFILE_BEGIN_SESSION("Runtime", "OverEngineProfile-Runtime.json");
	app->Run();
	OE_PROFILE_END_SESSION();

	OE_PROFILE_BEGIN_SESSION("Shutdown", "OverEngineProfile-Shutdown.json");
	delete app;
	OE_PROFILE_END_SESSION();
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
