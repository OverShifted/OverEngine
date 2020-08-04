#define OE_CLIENT_INCLUDE_ENTRY_POINT
#include <OverEngine.h>

#include "EditorLayer.h"

using namespace OverEngine;

class EditorApp : public Application
{
public:
	EditorApp(int argc, char** argv)
		: Application("OverEditor", true)
	{
		PushLayer(new EditorLayer());
		m_ImGuiEnabled = true;

		for (int i = 0; i < argc; i++)
		{
			OE_CORE_INFO(argv[i]);
		}
	}

	~EditorApp()
	{

	}
};

Application* OverEngine::CreateApplication(int argc, char** argv)
{
	return new EditorApp(argc, argv);
}