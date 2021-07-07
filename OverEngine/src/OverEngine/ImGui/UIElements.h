#pragma once

#include "Action.h"

#include "OverEngine/Core/AssetManagement/Asset.h"
#include "OverEngine/Scene/Entity.h"

#include "ExtraImGui.h"
#include <imgui.h>

namespace OverEngine
{
	class UIElements
	{
	public:
		template <typename T>
		static bool BeginComponentEditor(Entity entity, const char* headerName, uint32_t componentTypeID);

		static inline void BeginFieldGroup() { ImGui::Columns(2); }
		static inline void EndFieldGroup() { ImGui::Columns(1); }

		static bool CheckboxField(const char* fieldName, bool* value);


		// Useful for Vectors
		static bool DragFloatNField(const char* fieldName, float* value, uint32_t count = 1, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f");
		static bool DragFloatField(const char* fieldName, float* value, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f");
		static bool DragFloat2Field(const char* fieldName, float value[2], float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f");
		static bool DragFloat3Field(const char* fieldName, float value[3], float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f");
		static bool Color4Field(const char* fieldName, float value[4]);

		// Combobox useful for enums
		using EnumValues = Map<int, String>;

		template <typename T>
		static bool BasicEnum(const char* fieldName, const EnumValues& values, bool isFlag, T* currentValue, const ImGuiSelectableFlags& flags = 0);

		// Drag and drop
		static bool ObjectField(const char* fieldName, ObjectTypeInfo* typeInfo, Ref<Object>& object);
		static void Texture2DDragSource(const Ref<Texture2D>& texture, const char* name, bool preview = false);

		// Tooltip
		template <typename F>
		static void Tooltip(F func);

		////////////////////////////////////////////////////////
		/// Undo ables /////////////////////////////////////////
		////////////////////////////////////////////////////////

		static bool CheckboxField_U(const char* fieldName, const SelfContainedPropertyGetter& getter, const SelfContainedPropertySetter& setter);

		static bool DragFloatField_U(const char* fieldName, const SelfContainedPropertyGetter& getter, const SelfContainedPropertySetter& setter, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f");
		static bool DragFloat2Field_U(const char* fieldName, const SelfContainedPropertyGetter& getter, const SelfContainedPropertySetter& setter, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f");
		static bool DragFloat3Field_U(const char* fieldName, const SelfContainedPropertyGetter& getter, const SelfContainedPropertySetter& setter, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f");
		static bool Color4Field_U(const char* fieldName, const SelfContainedPropertyGetter& getter, const SelfContainedPropertySetter& setter);

		static bool BasicEnum_U(const char* fieldName, const EnumValues& values, bool isFlag, const SelfContainedPropertyGetter& getter, const SelfContainedPropertySetter& setter, const ImGuiSelectableFlags& flags = 0);
		static bool ObjectField_U(const char* fieldName, ObjectTypeInfo* typeInfo, const SelfContainedPropertyGetter& getter, const SelfContainedPropertySetter& setter);
	};

	////////////////////////////////////////////////////////
	/// Implementation /////////////////////////////////////
	////////////////////////////////////////////////////////

	template <typename T>
	bool UIElements::BeginComponentEditor(Entity entity, const char* headerName, uint32_t componentTypeID)
	{
		char txt[64];
		constexpr auto txtSize = OE_ARRAY_SIZE(txt);

		sprintf_s(txt, txtSize, "%s##%i", "X", componentTypeID);

		bool componentRemoved = false;
		if (ImGui::Button(txt))
		{
			entity.RemoveComponent<T>();
			componentRemoved = true;
		}

		sprintf_s(txt, txtSize, "##%i", componentTypeID);
		if (!componentRemoved)
		{
			ImGui::SameLine();
			ImGui::Checkbox(txt, &entity.GetComponent<T>().Enabled);
		}

		ImGui::SameLine();

		sprintf_s(txt, txtSize, "##MOVE_COMPONENT_UP%i", componentTypeID);

		if (ImGui::ArrowButton(txt, ImGuiDir_Up))
		{
			auto& componentList = entity.GetComponentsTypeIDList();
			auto it = STD_CONTAINER_FIND(componentList, componentTypeID);
			if (it != componentList.begin())
			{
				componentList[it - componentList.begin()] = componentList[it - componentList.begin() - 1];
				componentList[it - componentList.begin() - 1] = componentTypeID;
			}
		}

		ImGui::SameLine();

		sprintf_s(txt, txtSize, "##MOVE_COMPONENT_DOWN%i", componentTypeID);

		if (ImGui::ArrowButton(txt, ImGuiDir_Down))
		{
			auto& componentList = entity.GetComponentsTypeIDList();
			auto it = STD_CONTAINER_FIND(componentList, componentTypeID);
			if (it != componentList.end() - 1)
			{
				componentList[it - componentList.begin()] = componentList[it - componentList.begin() + 1];
				componentList[it - componentList.begin() + 1] = componentTypeID;
			}
		}

		ImGui::SameLine();
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		return !componentRemoved && ImGui::CollapsingHeader(headerName);
	}

	template <typename T>
	bool UIElements::BasicEnum(const char* fieldName, const EnumValues& values, bool isFlag, T* currentValue, const ImGuiSelectableFlags& flags)
	{
		ImGui::TextUnformatted(fieldName);
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		ImGui::PushID(fieldName);

		bool changed = false;

		if (isFlag)
		{
			if (isFlag)
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			for (const auto &value : values)
			{
				if (value.first != 0)
					changed |= ImGui::OE_CheckboxFlagsT(value.second.c_str(), currentValue, (T)value.first);
			}

			if (isFlag)
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		}
		else
		{
			if (ImGui::BeginCombo("", values.at((int)(*currentValue)).c_str()))
			{
				for (const auto &value : values)
				{
					const bool selected = (value.first == *currentValue);
					if (ImGui::Selectable(value.second.c_str(), selected, flags))
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
		}

		ImGui::PopID();
		ImGui::PopItemWidth();
		ImGui::NextColumn();

		return changed;
	}

	template <typename F>
	void UIElements::Tooltip(F func)
	{
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			func();
			ImGui::EndTooltip();
		}
	}
}
