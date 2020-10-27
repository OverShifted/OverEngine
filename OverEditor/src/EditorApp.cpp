#define OE_CLIENT_INCLUDE_ENTRY_POINT
#include <OverEngine.h>

#include "EditorLayer.h"

using namespace OverEngine;

static ApplicationProps GenApplicationProps()
{
	ApplicationProps props;
	props.MainWindowProps.Title = "OverEditor";
	props.MainWindowProps.Width = 1600;
	props.MainWindowProps.Height = 900;
	props.RuntimeType = RuntimeType::Editor;
	return props;
}

class EditorApp : public Application
{
public:
	EditorApp(int argc, char** argv)
		: Application(GenApplicationProps())
	{
		PushLayer(new OverEditor::EditorLayer());
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
