#pragma once

#include <imgui/imgui.h>
#include <string>

namespace ImGui
{
	bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0);
}