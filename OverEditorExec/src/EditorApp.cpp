#include <OverEngine.h>

#include "EditorLayer.h"

class EditorApp : public OverEngine::Application
{
public:
	EditorApp()
		:Application("OverEditor")
	{
		PushLayer(new OverEditor::EditorLayer());
	}

	~EditorApp()
	{

	}
};

OverEngine::Application* OverEngine::CreateApplication()
{
	return new EditorApp();
}