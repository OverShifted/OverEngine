#pragma once

#include <OverEngine/Layers/Layer.h>
#include <OverEngine/Events/Event.h>
#include <OverEngine/Core/Core.h>

namespace OverEditor
{
	class EditorLayer : public OverEngine::Layer
	{
	public:
		EditorLayer();
		
		void OnAttach() override;
		void OnUpdate() override;
		void OnImGuiRender() override;
		void OnEvent(OverEngine::Event& event) override;

	private:
		void MainDockSpace();
		void MainMenuBar();

	private:
		OverEngine::String vendorInfo, rendererInfo, versionInfo;
	};
}