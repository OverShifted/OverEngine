#pragma once

#include <OverEngine.h>
#include <OverEngine/ImGui/ImGuiPanel.h>
#include "Behavior/SceneEditor.h"

namespace OverEditor
{
	using namespace OverEngine;

	class SceneHierarchyPanel : public ImGuiPanel
	{
	public:
		SceneHierarchyPanel(const Ref<SceneEditor>& context = nullptr);

		virtual void OnImGuiRender() override;

		virtual void SetOpen(bool isOpen) override { m_IsOpen = isOpen; }
		virtual bool IsOpen() override { return m_IsOpen; }

		void SetContext(const Ref<SceneEditor>& context) { m_Context = context; }
		Ref<SceneEditor>& GetContext() { return m_Context; }
	private:
		Entity RecursiveDraw(Entity parentEntity = Entity());
	private:
		bool m_IsOpen;

		Ref<SceneEditor> m_Context;
	};
}
