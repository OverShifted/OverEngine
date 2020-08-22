#include "pcheader.h"
#include "OverEngine/ImGui/ExtraImGui.h"

#include <imgui/imgui.h>

namespace ImGui
{
	static int InputTextCallback(ImGuiInputTextCallbackData* data)
	{
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			// Resize string callback
			std::string* str = (std::string*)data->UserData;
			IM_ASSERT(data->Buf == str->c_str());
			str->resize(data->BufTextLen);
			data->Buf = (char*)str->c_str();
		}

		return 0;
	}

	bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags)
	{
		flags |= ImGuiInputTextFlags_CallbackResize;
		return InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, (void*)str);
	}
}