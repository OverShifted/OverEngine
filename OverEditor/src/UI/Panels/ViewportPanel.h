#pragma once

#include <OverEngine.h>
#include <OverEngine/ImGui/ImGuiPanel.h>
#include "Behavior/SceneEditor.h"

namespace OverEditor
{
	using namespace OverEngine;

	enum class ViewportTool
	{
		None, Translate2D
	};

	class ViewportPanel : public ImGuiPanel
	{
	public:
		ViewportPanel(const Ref<SceneEditor>& scene = nullptr);

		virtual void OnImGuiRender() override;
		void OnRender();

		virtual void SetIsOpen(bool isOpen) override { m_IsOpen = isOpen; }
		virtual bool GetIsOpen() override { return m_IsOpen; }

		void SetContext(const Ref<SceneEditor> context);
		Ref<SceneEditor>& GetContext() { return m_Context; }
	private:
		struct ViewportRay
		{
			Vector3 Origin = { 0.0f, 0.0f, 0.0f };
			Vector3 Direction = { 0.0f, 0.0f, 0.0f };
			float T = 0.0f;
		};

		float ClosestDistanceBetweenLines(ViewportRay& l1, ViewportRay& l2);
		void DrawGizmo(TransformComponent& entityTransform, bool hovered);

		void DrawGrid();
	private:
		bool m_IsOpen;

		// Rendering and viewport
		SceneCamera m_Camera;
		Transform m_CameraTransform;
		Ref<SceneEditor> m_Context;
		Ref<FrameBuffer> m_FrameBuffer;
		Vector2 m_PanelSize = { 0, 0 };
		Vector2 m_PanelPos = { 0, 0 };

		// Navigation
		bool m_FDownLastFrame = false;
		bool m_Panning = false;
		Vector2 m_LastMouseDelta = { 0, 0 };

		// Tools
		ViewportTool m_Tool = ViewportTool::Translate2D;

		Axis m_HoveredTranslateAxis = Axis::None;
		Axis m_ActiveTranslateAxis = Axis::None;
		Vector3 m_Translate2DToolLastIntersect = { 0.0f, 0.0f, 0.0f };
	};
}
