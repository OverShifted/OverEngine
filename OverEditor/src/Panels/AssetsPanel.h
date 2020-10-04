#pragma once

#include <OverEngine.h>
#include <OverEngine/ImGui/ImGuiPanel.h>

namespace OverEditor
{
	using namespace OverEngine;
	class Editor;

	class AssetsPanel : public ImGuiPanel
	{
	public:
		AssetsPanel(Editor* editor);

		virtual void OnImGuiRender() override;

		virtual void SetIsOpen(bool isOpen) override { m_IsOpen = isOpen; }
		virtual bool GetIsOpen() override { return m_IsOpen; }
	private:
		Ref<Resource> RecursiveDraw(const Ref<Resource>& resourceToDraw);
	private:
		bool m_IsOpen;

		Editor* m_Editor;
		Vector<Ref<Resource>> m_SelectionContext;

		bool m_OneColumnView = false;
		int m_ThumbnailSize = 100;
		int m_ThumbnailSizeMin = 50;
		int m_ThumbnailSizeMax = 300;
	};
}
