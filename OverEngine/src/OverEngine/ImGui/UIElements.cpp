#include "pcheader.h"
#include "UIElements.h"

namespace OverEngine
{
	// TODO: Fast Random ImGui IDs
	bool UIElements::CheckboxField(const char* fieldName, bool* value)
	{
		ImGui::TextUnformatted(fieldName);
		ImGui::NextColumn();
		ImGui::PushID(fieldName);

		bool changed = ImGui::Checkbox("", value);

		ImGui::PopID();
		ImGui::NextColumn();
		return changed;
	}

	bool UIElements::DragFloatNField(const char* fieldName, float* value, uint32_t count, float speed, float min, float max, const char* format)
	{
		ImGui::TextUnformatted(fieldName);
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		ImGui::PushID(fieldName);

		bool changed = ImGui::DragScalarN("", ImGuiDataType_Float, value, count, speed, &min, &max, format);

		ImGui::PopID();
		ImGui::PopItemWidth();
		ImGui::NextColumn();
		return changed;
	}

	bool UIElements::DragFloatField(const char* fieldName, float* value, float speed, float min, float max, const char* format)
	{
		return DragFloatNField(fieldName, value, 1, speed, min, max, format);
	}

	bool UIElements::DragFloat2Field(const char* fieldName, float value[2], float speed, float min, float max, const char* format)
	{
		return DragFloatNField(fieldName, value, 2, speed, min, max, format);
	}

	bool UIElements::DragFloat3Field(const char* fieldName, float value[3], float speed, float min, float max, const char* format)
	{
		return DragFloatNField(fieldName, value, 3, speed, min, max, format);
	}

	bool UIElements::Color4Field(const char* fieldName, float value[4])
	{
		ImGui::TextUnformatted(fieldName);
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		ImGui::PushID(fieldName);

		bool changed = ImGui::ColorEdit4("", value);

		ImGui::PopID();
		ImGui::PopItemWidth();
		ImGui::NextColumn();
		return changed;
	}

	bool UIElements::ObjectField(const char* fieldName, ObjectTypeInfo* typeInfo, Ref<Object>& object)
	{
		ImGui::TextUnformatted(fieldName);
		ImGui::NextColumn();

		bool changed = false;
		char fieldText[128];
		size_t fieldTextSize = sizeof(fieldText) / sizeof(char);

		if (object)
		{
			Variant objName;
			if (object->GetProperty("Name", &objName))
				sprintf_s(fieldText, fieldTextSize, "%s (%s)", objName.operator const String&().c_str(), typeInfo->Name.c_str());
			else
				sprintf_s(fieldText, fieldTextSize, "NameLess (%s)", typeInfo->Name.c_str());
		}
		else
		{
			sprintf_s(fieldText, fieldTextSize, "None (%s)", typeInfo->Name.c_str());
		}

		ImGui::PushItemWidth(object ? ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("X").x - ImGui::GetStyle().FramePadding.x * 3 : -1);
		ImGui::PushID(fieldName);

		ImGui::InputText("", fieldText, fieldTextSize, ImGuiInputTextFlags_ReadOnly);

		ImGui::PopID();
		ImGui::PopItemWidth();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(typeInfo->Name.c_str()))
			{
				object = *static_cast<Ref<Texture2D>*>(payload->Data);
				changed = true;
			}
			ImGui::EndDragDropTarget();
		}

		if (object)
		{
			ImGui::SameLine();
			if (ImGui::Button("X"))
			{
				object = nullptr;
				changed = true;
			}
		}

		ImGui::NextColumn();

		return changed;
	}

	void UIElements::Texture2DDragSource(const Ref<Texture2D>& texture, const char* name, bool preview)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 10.0f, 10.0f });
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("Texture2D", &texture, sizeof(Ref<Texture2D>));

			ImGui::TextUnformatted(name);
			if (preview)
				ImGui::Image(texture, { 128, 128 });

			ImGui::EndDragDropSource();
		}
		ImGui::PopStyleVar();
	}

	bool UIElements::CheckboxField_U(const char* fieldName, const SelfContainedPropertyGetter& getter, const SelfContainedPropertySetter& setter)
	{
		bool value = getter();
		bool changed = CheckboxField(fieldName, &value);
		if (changed)
			ActionStack::GetActiveInstance().Do(CreateRef<Action>(setter, !value, value));

		return changed;
	}

	bool UIElements::DragFloatField_U(const char* fieldName, const SelfContainedPropertyGetter& getter, const SelfContainedPropertySetter& setter, float speed, float min, float max, const char* format)
	{
		static float delta = 0.0f;

		// Backup value
		float firstValue = getter();
		float secondValue = firstValue;

		bool changed = false;
		if (UIElements::DragFloatField(fieldName, &secondValue, speed, min, max, format))
		{
			delta += secondValue - firstValue;
			setter(secondValue);
			changed = true;
		}

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			ActionStack::GetActiveInstance().Do(CreateRef<Action>(setter, firstValue - delta, firstValue), false);
			delta = 0.0f;
		}

		return changed;
	}

	bool UIElements::DragFloat2Field_U(const char* fieldName, const SelfContainedPropertyGetter& getter, const SelfContainedPropertySetter& setter, float speed, float min, float max, const char* format)
	{
		static Vector2 delta(0.0f);

		// Backup value
		Vector2 firstValue = getter();
		Vector2 secondValue = firstValue;

		bool changed = false;
		if (UIElements::DragFloat2Field(fieldName, glm::value_ptr(secondValue), speed, min, max, format))
		{
			delta += secondValue - firstValue;
			setter(secondValue);
			changed = true;
		}

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			ActionStack::GetActiveInstance().Do(CreateRef<Action>(setter, firstValue - delta, firstValue), false);
			delta = Vector2(0.0f);
		}

		return changed;
	}

	bool UIElements::DragFloat3Field_U(const char* fieldName, const SelfContainedPropertyGetter& getter, const SelfContainedPropertySetter& setter, float speed, float min, float max, const char* format)
	{
		static Vector3 delta(0.0f);

		// Backup value
		Vector3 firstValue = getter();
		Vector3 secondValue = firstValue;

		bool changed = false;
		if (UIElements::DragFloat3Field(fieldName, glm::value_ptr(secondValue), speed, min, max, format))
		{
			delta += secondValue - firstValue;
			setter(secondValue);
			changed = true;
		}

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			ActionStack::GetActiveInstance().Do(CreateRef<Action>(setter, firstValue - delta, firstValue), false);
			delta = Vector3(0.0f);
		}

		return changed;
	}

	bool UIElements::Color4Field_U(const char* fieldName, const SelfContainedPropertyGetter& getter, const SelfContainedPropertySetter& setter)
	{
		static Vector4 delta(0.0f);

		// Backup value
		Vector4 firstValue = getter();
		Vector4 secondValue = firstValue;

		bool changed = false;
		if (UIElements::Color4Field(fieldName, glm::value_ptr(secondValue)))
		{
			delta += secondValue - firstValue;
			setter(secondValue);
			changed = true;
		}

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			ActionStack::GetActiveInstance().Do(CreateRef<Action>(setter, firstValue - delta, firstValue), false);
			delta = Vector4(0.0f);
		}

		return changed;
	}

	bool UIElements::BasicEnum_U(const char* fieldName, const EnumValues& values, bool isFlag, const SelfContainedPropertyGetter& getter, const SelfContainedPropertySetter& setter, const ImGuiSelectableFlags& flags)
	{
		Variant firstValue = getter();
		int secondValueInt = firstValue;
		bool changed = BasicEnum(fieldName, values, isFlag, &secondValueInt, flags);

		Variant secondValue = secondValueInt;
		if (changed)
			ActionStack::GetActiveInstance().Do(CreateRef<Action>(setter, firstValue, secondValue));

		return changed;
	}

	bool UIElements::ObjectField_U(const char* fieldName, ObjectTypeInfo* typeInfo, const SelfContainedPropertyGetter& getter, const SelfContainedPropertySetter& setter)
	{
		Ref<Object> firstValue;
		Variant firstVariantValue = getter();
		if (firstVariantValue.GetType())
			firstValue = (Ref<Object>)firstVariantValue;
		Ref<Object> secondValue = firstValue;

		bool changed = ObjectField(fieldName, typeInfo, secondValue);
		if (changed)
			ActionStack::GetActiveInstance().Do(CreateRef<Action>(setter, (Ref<Object>)firstValue, (Ref<Object>)secondValue));

		return changed;
	}
}
