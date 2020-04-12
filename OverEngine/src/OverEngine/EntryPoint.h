#pragma once
#include"pcheader.h"
#ifdef OE_PLATFORM_WINDOWS

extern  OverEngine::Application* OverEngine::CreateApplication();

/*int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)//*/ int main(int argc, char** argv)
{
	OverEngine::Log::Init();
	OE_CORE_INFO("OverEngine v0.0");

	auto app = OverEngine::CreateApplication();
	app->Run();
	delete app;
}
#endif // OE_PLATFORM_WINDOWS
