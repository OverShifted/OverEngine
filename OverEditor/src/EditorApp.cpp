#define OE_CLIENT_INCLUDE_ENTRY_POINT
#include <OverEngine.h>

#include "EditorLayer.h"

class EditorApp : public OverEngine::Application
{
public:
	EditorApp()
		:Application("OverEditor", false)
	{
		PushLayer(new OverEditor::EditorLayer());
		m_ImGuiEnabled = true;
	}

	~EditorApp()
	{

	}
};

OverEngine::Application* OverEngine::CreateApplication()
{
	return new EditorApp();
}