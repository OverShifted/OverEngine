#include "EditorLayer.h"

#include "Editor.h"
#include <OverEngine/Core/GUIDGenerator.h>

#include <imgui/imgui.h>

EditorLayer* EditorLayer::s_Instance = nullptr;

EditorLayer::EditorLayer()
	: Layer("EditorLayer")
{
	OE_PROFILE_FUNCTION();

	s_Instance = this;
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
	ImGui::ShowDemoWindow();
}

void EditorLayer::OnEvent(Event& event)
{
	OE_PROFILE_FUNCTION();

	m_Editor.OnEvent(event);
}
