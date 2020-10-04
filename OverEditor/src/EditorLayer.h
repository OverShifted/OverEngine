#pragma once

#include "Editor.h"
#include <OverEngine.h>

namespace OverEditor
{
	using namespace OverEngine;

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		void OnAttach() override {};
		void OnUpdate(TimeStep DeltaTime) override;
		void OnImGuiRender() override;
		void OnEvent(Event& event) override;
	private:
		Editor m_Editor;
	};
}
