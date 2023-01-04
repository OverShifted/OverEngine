#pragma once

#include "OverEngine/Renderer/Texture.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <string>

namespace ImGui
{
	using namespace OverEngine;

	bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0);

	void Image(const Ref<Texture2D> texture, const ImVec2& size, const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));
	bool ImageButton(const Ref<Texture2D> texture, const ImVec2& size, int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

	template<typename T>
	bool OE_CheckboxFlagsT(const char* label, T* flags, T flags_value)
	{
		bool all_on = (*flags & flags_value) == flags_value;
		bool any_on = (*flags & flags_value) != 0;
		bool pressed;
		if (!all_on && any_on)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, true);
			pressed = Checkbox(label, &all_on);
			ImGui::PopItemFlag();
		}
		else
		{
			pressed = Checkbox(label, &all_on);

		}
		if (pressed)
		{
			if (all_on)
				*flags |= flags_value;
			else
				*flags &= ~flags_value;
		}
		return pressed;
	}
}
