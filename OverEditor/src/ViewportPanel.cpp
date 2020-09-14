#include "ViewportPanel.h"
#include <imgui/imgui.h>

ViewportPanel::ViewportPanel(Ref<Scene> context)
	: m_Context(context)
{
	FrameBufferProps fbProps;
	fbProps.Width = 720;
	fbProps.Height = 1280;
	m_FrameBuffer = FrameBuffer::Create(fbProps);
}

void ViewportPanel::OnImGuiRender()
{
	if (m_Context && m_SceneRendered)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });

	ImGui::Begin("Viewport");

	if (m_Context && m_SceneRendered)
	{
		ImGui::PopStyleVar();
		ImVec2 panelSize = ImGui::GetContentRegionAvail();
		m_PanelSize = { panelSize.x, panelSize.y };

		ImGui::Image((void*)(intptr_t)m_FrameBuffer->GetColorAttachmentRendererID(), panelSize, { 0, 1 }, { 1, 0 });
	}
	else if (m_Context)
	{
		ImGui::TextUnformatted("No camera is rendering");
	}
	else
	{
		ImGui::TextUnformatted("Open a Scene to start editing");
	}

	ImGui::End();
}

void ViewportPanel::OnRender()
{
	if (m_Context)
	{
		if (FrameBufferProps props = m_FrameBuffer->GetProps();
			m_PanelSize.x > 0.0f && m_PanelSize.y > 0.0f && // zero sized framebuffer is invalid
			(props.Width != m_PanelSize.x || props.Height != m_PanelSize.y))
		{
			m_FrameBuffer->Resize((uint32_t)m_PanelSize.x, (uint32_t)m_PanelSize.y);
		}
	}

	m_FrameBuffer->Bind();

	if (m_Context)
	{
		m_SceneRendered = m_Context->OnRender(m_PanelSize);
	}
	else
	{
		RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		RenderCommand::Clear();
	}

	m_FrameBuffer->Unbind();
}

void ViewportPanel::SetContext(const Ref<Scene> context)
{
	m_Context = context;
}
