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

	void Image(const Ref<Texture2D> texture, const ImVec2& size, const ImVec4& tint_col, const ImVec4& border_col)
	{
		Rect textureRect = texture->GetRect();
		Image((void*)(intptr_t)texture->GetGPUTexture()->GetRendererID(), size, { textureRect.x, textureRect.y }, { textureRect.z + textureRect.x, textureRect.w + textureRect.y }, tint_col, border_col);
	}

	bool ImageButton(const Ref<Texture2D> texture, const ImVec2& size, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
	{
		Rect textureRect = texture->GetRect();
		return ImageButton((void*)(intptr_t)texture->GetGPUTexture()->GetRendererID(), size, { textureRect.x, textureRect.y }, { textureRect.z + textureRect.x, textureRect.w + textureRect.y }, frame_padding, bg_col, tint_col);
	}
}
