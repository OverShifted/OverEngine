#pragma once

#include <OverEngine.h>
#include <OverEngine/ImGui/ImGuiPanel.h>
#include <OverEngine/Core/AssetManagement/AssetDatabase.h>

#include <variant>

namespace OverEditor
{
	using namespace OverEngine;

	class AssetsPanel : public ImGuiPanel
	{
	public:
		AssetsPanel();

		virtual void OnImGuiRender() override;

		virtual void SetIsOpen(bool isOpen) override { m_IsOpen = isOpen; }
		virtual bool GetIsOpen() override { return m_IsOpen; }
	private:
		Ref<Asset> RecursiveDraw(const Ref<AssetFolder>& resourceToDraw);
		void DrawThumbnail(Ref<Asset> asset, bool last);
	private:
		bool m_IsOpen = true;

		Ref<Asset> m_SelectionContext;

		bool m_OneColumnView = false;
		uint32_t m_ThumbnailSize = 100;
		uint32_t m_ThumbnailSizeMin = 50;
		uint32_t m_ThumbnailSizeMax = 300;
	};
}
