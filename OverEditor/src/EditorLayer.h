#pragma once

#include "EditorProject.h"
#include <OverEngine.h>

#include <OverEngine/Assets/Asset.h>
#include <OverEngine/Assets/AssetCollection.h>
#include <OverEngine/Core/FileSystem/FileSystem.h>

#include "Action.h"
#include "SceneEditor.h"
#include "Panels/ViewportPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/AssetsPanel.h"

namespace OverEditor
{
	using namespace OverEngine;

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		void OnAttach() override;
		void OnUpdate(TimeStep DeltaTime) override;
		void OnImGuiRender() override;
		void OnEvent(Event& event) override;

		inline auto& GetProject()     { return m_EditingProject; }
		inline auto& GetIcons()       { return m_Icons; }
		inline auto& GetActionStack() { return m_ActionStack; }

		void EditScene(const Ref<SceneAsset>& sceneAsset);

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
		ConsolePanel m_ConsolePanel;
		AssetsPanel m_AssetsPanel;

		bool m_IsProjectManagerOpen = true;
	};
}
