#pragma once

#include <OverEngine.h>
#include <OverEngine/ImGui/ImGuiPanel.h>

using namespace OverEngine;

class ViewportPanel : public ImGuiPanel
{
public:
	ViewportPanel(Ref<Scene> context = nullptr);

	virtual void OnImGuiRender() override;
	void OnRender();

	void SetContext(const Ref<Scene> context);
private:
	Ref<Scene> m_Context = nullptr;
	Ref<FrameBuffer> m_FrameBuffer;
	Vector2 m_PanelSize = { 0, 0 };
	bool m_SceneRendered = false;
};
