#include "UIElements.h"

namespace OverEditor
{
	bool UIElements::CheckboxField(const char* fieldName, const char* fieldID, bool* value)
	{
		ImGui::TextUnformatted(fieldName);
		ImGui::NextColumn();

		bool changed = ImGui::Checkbox(fieldID, value);
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

	void UIElements::Texture2DDragSource(const Ref<Texture2D>& texture, const char* name, bool preview)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 10.0f, 10.0f });
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("_TEXTURE2D_DRAG", &texture, sizeof(Ref<Texture2D>));

			ImGui::TextUnformatted(name);
			if (preview)
				ImGui::Image(texture, { 128, 128 });

			ImGui::EndDragDropSource();
		}
		ImGui::PopStyleVar();
	}

	bool UIElements::CheckboxField_U(const char* fieldName, const char* fieldID, bool* value, const BoolEditAction::GetterFn& getter, const BoolEditAction::SetterFn& setter)
	{
		bool changed = CheckboxField(fieldName, fieldID, value);
		if (changed)
			EditorLayer::Get().GetActionStack().Do(CreateRef<BoolEditAction>(getter, setter), false);

		return changed;
	}

	bool UIElements::DragFloatField_U(const char* fieldName, const char* fieldID, float* value, const FloatEditAction::GetterFn& getter, const FloatEditAction::SetterFn& setter, float speed /*= 1.0f*/, float min /*= 0.0f*/, float max /*= 0.0f*/, const char* format /*= "%.3f"*/)
	{
		static float delta = 0.0f;

		// Backup value's value
		float v = *value;
		float valueToChange = v;

		bool changed = false;
		if (UIElements::DragFloatField(fieldName, fieldID, &valueToChange, speed, min, max, format))
		{
			delta += valueToChange - v;
			changed = true;
		}

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			EditorLayer::Get().GetActionStack().Do(CreateRef<FloatEditAction>(delta, getter, setter), false);
			delta = 0.0f;
		}

		if (changed)
		{
			*value = valueToChange;
		}

		return changed;
	}

	bool UIElements::DragFloat2Field_U(const char* fieldName, const char* fieldID, float value[2], const Vector2EditAction::GetterFn& getter, const Vector2EditAction::SetterFn& setter, float speed /*= 1.0f*/, float min /*= 0.0f*/, float max /*= 0.0f*/, const char* format /*= "%.3f"*/)
	{
		static Vector2 delta(0.0f);

		// Backup value's value
		Vector2 v{ value[0], value[1], };
		Vector2 valueToChange = v;

		bool changed = false;
		if (UIElements::DragFloat2Field(fieldName, fieldID, glm::value_ptr(valueToChange), speed, min, max, format))
		{
			delta += valueToChange - v;
			changed = true;
		}

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			EditorLayer::Get().GetActionStack().Do(CreateRef<Vector2EditAction>(delta, getter, setter), false);
			delta = Vector2(0.0f);
		}

		if (changed)
		{
			value[0] = valueToChange[0];
			value[1] = valueToChange[1];
		}

		return changed;
	}

	bool UIElements::DragFloat3Field_U(const char* fieldName, const char* fieldID, float value[3], const Vector3EditAction::GetterFn& getter, const Vector3EditAction::SetterFn& setter, float speed /*= 1.0f*/, float min /*= 0.0f*/, float max /*= 0.0f*/, const char* format /*= "%.3f"*/)
	{
		static Vector3 delta(0.0f);

		// Backup value's value
		Vector3 v{ value[0], value[1], value[2] };
		Vector3 valueToChange = v;

		bool changed = false;
		if (UIElements::DragFloat3Field(fieldName, fieldID, glm::value_ptr(valueToChange), speed, min, max, format))
		{
			delta += valueToChange - v;
			changed = true;
		}

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			EditorLayer::Get().GetActionStack().Do(CreateRef<Vector3EditAction>(delta, getter, setter), false);
			delta = Vector3(0.0f);
		}

		if (changed)
		{
			value[0] = valueToChange[0];
			value[1] = valueToChange[1];
			value[2] = valueToChange[2];
		}

		return changed;
	}

	bool UIElements::Color4Field_U(const char* fieldName, const char* fieldID, float value[4], const Vector4EditAction::GetterFn& getter, const Vector4EditAction::SetterFn& setter)
	{
		static Vector4 delta(0.0f);

		// Backup value's value
		Vector4 v{ value[0], value[1], value[2], value[3] };
		Vector4 valueToChange = v;

		bool changed = false;
		if (UIElements::Color4Field(fieldName, fieldID, glm::value_ptr(valueToChange)))
		{
			delta += valueToChange - v;
			changed = true;
		}

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			EditorLayer::Get().GetActionStack().Do(CreateRef<Vector4EditAction>(delta, getter, setter), false);
			delta = Vector4(0.0f);
		}

		if (changed)
		{
			value[0] = valueToChange[0];
			value[1] = valueToChange[1];
			value[2] = valueToChange[2];
			value[3] = valueToChange[3];
		}

		return changed;
	}
}
