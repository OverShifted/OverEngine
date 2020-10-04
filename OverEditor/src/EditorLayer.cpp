#include "EditorLayer.h"

#include "Editor.h"
#include "EditorConsoleSink.h"

namespace OverEditor
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
		OE_PROFILE_FUNCTION();

		auto editorConsoleSink = std::make_shared<EditorConsoleSink_mt>(&m_Editor.m_ConsolePanel);
		editorConsoleSink->set_pattern("%v");
		OverEngine::Log::GetCoreLogger()->sinks().push_back(editorConsoleSink);
		OverEngine::Log::GetClientLogger()->sinks().push_back(editorConsoleSink);
	}

	void EditorLayer::OnUpdate(TimeStep DeltaTime)
	{
		OE_PROFILE_FUNCTION();

		m_Editor.OnUpdate();
	}

	void EditorLayer::OnImGuiRender()
	{
		OE_PROFILE_FUNCTION();

		m_Editor.OnImGuiRender();
	}

	void EditorLayer::OnEvent(Event& event)
	{
		OE_PROFILE_FUNCTION();
	}
}
