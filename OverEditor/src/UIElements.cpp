#include "UIElements.h"

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

	void UIElements::Texture2DField(const char* fieldName, const char* fieldID, Ref<Texture2D>& texture)
	{
		ImGui::TextUnformatted(fieldName);
		ImGui::NextColumn();

		char txt[128];
		auto txtSize = sizeof(txt) / sizeof(char);

		if (!texture)
			strcpy_s(txt, txtSize, "None (Texture2D)");
		else
			strcpy_s(txt, txtSize, texture->GetName().c_str());

		if (!texture)
			ImGui::PushItemWidth(-1);
		else
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("X").x - ImGui::GetStyle().FramePadding.x * 3);

		ImGui::InputText(fieldID, txt, txtSize, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE2D_DRAG"))
			{
				Ref<Texture2D>& incomingTexture = *static_cast<Ref<Texture2D>*>(payload->Data);
				texture = incomingTexture;
			}
			ImGui::EndDragDropTarget();
		}

		if (texture)
		{
			ImGui::SameLine();
			if (ImGui::Button("X"))
			texture = nullptr;
		}

		ImGui::NextColumn();
	}
}
