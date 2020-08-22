#pragma once

#include "Editor.h"

#include <OverEngine.h>

using namespace OverEngine;

class EditorLayer : public Layer
{
public:
	EditorLayer();
		
	void OnAttach() override {};
	void OnUpdate(TimeStep DeltaTime) override;
	void OnImGuiRender() override;
	void OnEvent(Event& event) override;

	static EditorLayer& Get() { return *s_Instance; }
	Editor& GetEditor() { return m_Editor; }
private:
	static EditorLayer* s_Instance;

	Editor m_Editor;
};