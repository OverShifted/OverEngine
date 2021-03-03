#pragma once

#include "pcheader.h"

#include <fstream>
#include <stdlib.h>

extern OverEngine::Application* OverEngine::CreateApplication(int argc, char** argv);

inline int OverMain(int argc, char** argv) // Platform agnostic entry point
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

/*#include <shellapi.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int numArgs = 0;
	LPWSTR* args = CommandLineToArgvW(lpCmdLine, &numArgs);

	char** buffer = malloc(numArgs * sizeof(char*));

	for (size_t i = 0; i < numArgs; i++)
	{
		auto len = wcslen(args[i]);
		buffer[i] = (char*)(malloc(len * sizeof(char)));
		wcstombs(buffer[i], args[i], len);
	}

	return OverMain(numArgs, buffer);
}*/

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
