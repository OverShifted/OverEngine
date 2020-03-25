#pragma once
#ifdef OE_PLATFORM_WINDOWS

extern  OverEngine::Application* OverEngine::CreateApplication();

int main(int argc, char** argv)
{
	OverEngine::Log::Init();
	OE_CORE_INFO("OverEngine preRealease Build (Debug)");

	auto app = OverEngine::CreateApplication();
	app->Run();
	delete app;
}
#endif // OE_PLATFORM_WINDOWS
