#include "UIElements.h"

#include <OverEngine/Assets/Asset.h>
#include <OverEngine/Assets/Resource.h>

namespace OverEditor
{
	bool UIElements::CheckboxField(const char* fieldName, const char* fieldID, bool* value)
	{
		ImGui::TextUnformatted(fieldName);
		ImGui::NextColumn();

		ImGui::PushItemWidth(-1);
		bool changed = ImGui::Checkbox(fieldID, value);
		ImGui::PopItemWidth();
		ImGui::NextColumn();
		return changed;
	}

	bool UIElements::DragFloatNField(const char* fieldName, const char* fieldID, float* value, uint32_t count, float speed, float min, float max, const char* format)
	{
		ImGui::TextUnformatted(fieldName);
		ImGui::NextColumn();

		ImGui::PushItemWidth(-1);
		bool changed = ImGui::DragScalarN(fieldID, ImGuiDataType_Float, value, count, speed, &min, &max, format);
		ImGui::PopItemWidth();
		ImGui::NextColumn();
		return changed;
	}

	bool UIElements::DragFloatField(const char* fieldName, const char* fieldID, float* value, float speed, float min, float max, const char* format)
	{
		return DragFloatNField(fieldName, fieldID, value, 1, speed, min, max, format);
	}

	bool UIElements::DragFloat2Field(const char* fieldName, const char* fieldID, float value[2], float speed, float min, float max, const char* format)
	{
		return DragFloatNField(fieldName, fieldID, value, 2, speed, min, max, format);
	}

	bool UIElements::DragFloat3Field(const char* fieldName, const char* fieldID, float value[3], float speed, float min, float max, const char* format)
	{
		return DragFloatNField(fieldName, fieldID, value, 3, speed, min, max, format);
	}

	bool UIElements::DragFloat4Field(const char* fieldName, const char* fieldID, float value[4], float speed, float min, float max, const char* format)
	{
		return DragFloatNField(fieldName, fieldID, value, 4, speed, min, max, format);
	}

	bool UIElements::Color4Field(const char* fieldName, const char* fieldID, float value[4])
	{
		ImGui::TextUnformatted(fieldName);
		ImGui::NextColumn();

		ImGui::PushItemWidth(-1);
		bool changed = ImGui::ColorEdit4(fieldID, value);
		ImGui::PopItemWidth();
		ImGui::NextColumn();
		return changed;
	}

	void UIElements::Texture2DAssetField(const char* fieldName, const char* fieldID, Ref<Texture2DAsset>& asset)
	{
		ImGui::TextUnformatted(fieldName);
		ImGui::NextColumn();

		char txt[128];
		auto txtSize = sizeof(txt) / sizeof(char);

		if (!asset)
			sprintf_s(txt, txtSize, "None (Texture2DAsset)");
		else if (asset->GetResource()) // Asset belong to a resource
			sprintf_s(txt, txtSize, "%s.%s",
				asset->GetResource()->GetName().c_str(),
				asset->GetName().c_str());
		else
			sprintf_s(txt, txtSize, "%s", asset->GetName().c_str());

		if (!asset)
			ImGui::PushItemWidth(-1);
		else
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("X").x - ImGui::GetStyle().FramePadding.x * 3);

		ImGui::InputText(fieldID, txt, txtSize, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_ASSET_DRAG"))
			{
				Ref<Texture2DAsset>& incomingAsset = *static_cast<Ref<Texture2DAsset>*>(payload->Data);
				asset = incomingAsset;
			}
			ImGui::EndDragDropTarget();
		}

		if (asset)
		{
			ImGui::SameLine();
			if (ImGui::Button("X"))
			asset = nullptr;
		}

		ImGui::NextColumn();
	}

	bool UIElements::BasicEnum(const char* fieldName, const char* fieldID, EnumValues& values, int* currentValue)
	{
		ImGui::TextUnformatted(fieldName);
		ImGui::NextColumn();

		ImGui::PushItemWidth(-1);
		bool changed = false;
		if (ImGui::BeginCombo(fieldID, values[*currentValue].c_str()))
		{
			for (const auto& value : values)
			{
				const bool selected = (value.first == *currentValue);
				if (ImGui::Selectable(value.second.c_str(), selected))
				{
					*currentValue = value.first;
					changed = true;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		ImGui::NextColumn();

		return changed;
	}
}
