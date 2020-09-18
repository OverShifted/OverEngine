#pragma once

#include <OverEngine/Scene/Entity.h>
#include <imgui.h>

using namespace OverEngine;

namespace OverEngine
{
	class Texture2DAsset;
}

namespace OverEditor
{
	/*
	 * A class which contains OverEditor's common
	 * Graphical User Interface elements.
	 */

	class UIElements
	{
	public:
		template <typename T>
		bool static BeginComponentEditor(Entity entity, const char* headerName, uint32_t componentTypeID)
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
				auto it = std::find(componentList.begin(), componentList.end(), componentTypeID);
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
				auto it = std::find(componentList.begin(), componentList.end(), componentTypeID);
				if (it != componentList.end() - 1)
				{
					componentList[it - componentList.begin()] = componentList[it - componentList.begin() + 1];
					componentList[it - componentList.begin() + 1] = componentTypeID;
				}
			}

			ImGui::SameLine();
			return !componentRemoved && ImGui::CollapsingHeader(headerName);
		}

		void static BeginFieldGroup() { ImGui::Columns(2); }
		void static EndFieldGroup() { ImGui::Columns(1); }

		static bool CheckboxField(const char* fieldName, const char* fieldID, bool* value);

		// Useful for Vectors .etc
		static bool DragFloatNField(const char* fieldName, const char* fieldID, float* value, uint32_t count = 1, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f");
		static bool DragFloatField(const char* fieldName, const char* fieldID, float* value, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f");
		static bool DragFloat2Field(const char* fieldName, const char* fieldID, float value[2], float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f");
		static bool DragFloat3Field(const char* fieldName, const char* fieldID, float value[3], float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f");
		static bool DragFloat4Field(const char* fieldName, const char* fieldID, float value[4], float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f");

		// For RGBA colors
		static bool Color4Field(const char* fieldName, const char* fieldID, float value[4]);

		// For Drag and Drop Asset fields
		static void Texture2DAssetField(const char* fieldName, const char* fieldID, Ref<Texture2DAsset>& asset);

		// Combobox useful for enums
		using EnumValues = Map<int, String>;
		static bool BasicEnum(const char* fieldName, const char* fieldID, EnumValues& values, int* currentValue);
	};
}
