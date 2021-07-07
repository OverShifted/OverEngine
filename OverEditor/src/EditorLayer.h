#pragma once

#include "Behavior/EditorProject.h"
#include <OverEngine.h>

#include <OverEngine/Core/AssetManagement/Asset.h>
#include <OverEngine/Core/AssetManagement/AssetDatabase.h>
#include <OverEngine/Core/FileSystem/FileSystem.h>
#include <OverEngine/ImGui/Action.h>

#include "Behavior/SceneEditor.h"
#include "UI/Panels/ViewportPanel.h"
#include "UI/Panels/SceneHierarchyPanel.h"
#include "UI/Panels/AssetsPanel.h"

namespace OverEditor
{
	using namespace OverEngine;

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		void OnAttach() override;
		void OnUpdate(TimeStep deltaTime) override;
		void OnImGuiRender() override;
		void OnEvent(Event& event) override;

		inline auto& GetProject()     { return m_EditingProject; }
		inline auto& GetIcons()       { return m_Icons; }
		inline auto& GetActionStack() { return m_ActionStack; }

		void EditScene(const Ref<Scene>& scene);

		static EditorLayer& Get() { return *s_Instance; }
	private:
		void OnProjectManagerGUI();
	private:
		static EditorLayer* s_Instance;

		ActionStack m_ActionStack;

		Ref<EditorProject> m_EditingProject;
		Ref<SceneEditor> m_SceneContext;

		// Editor Icons
		Ref<Texture2D> m_IconsTexture;
		UnorderedMap<String, Ref<Texture2D>> m_Icons;

		// Panels
		ViewportPanel m_ViewportPanel;
		SceneHierarchyPanel m_SceneHierarchyPanel;
		AssetsPanel m_AssetsPanel;

		bool m_IsProjectManagerOpen = true;
	};
}
