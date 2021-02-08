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
			auto str = reinterpret_cast<std::string*>(data->UserData);
			IM_ASSERT(data->Buf == str->c_str());
			str->resize(data->BufTextLen);
			data->Buf = str->data();
		}

		return 0;
	}

	bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags)
	{
		flags |= ImGuiInputTextFlags_CallbackResize;
		return ImGui::InputText(label, str->data(), str->capacity() + 1, flags, InputTextCallback, reinterpret_cast<void*>(str));
	}

	void Image(const Ref<Texture2D> texture, const ImVec2& size, const ImVec4& tint_col, const ImVec4& border_col)
	{
		TextureType textureType = texture->GetType();

		if (textureType == TextureType::Master)
		{
			ImGui::Image(reinterpret_cast<void*>(texture->GetRendererID()), size, { 0, 0 }, { 1, 1 }, tint_col, border_col);
		}
		else if (textureType == TextureType::SubTexture)
		{
			if (auto subTexture = std::dynamic_pointer_cast<SubTexture2D>(texture))
			{
				Rect rect = subTexture->GetRect();

				ImGui::Image(
					reinterpret_cast<void*>(texture->GetRendererID()), size,
					{ rect.x, rect.y }, { rect.z + rect.x, rect.w + rect.y },
					tint_col, border_col
				);
			}
		}
	}

	bool ImageButton(const Ref<Texture2D> texture, const ImVec2& size, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
	{
		TextureType textureType = texture->GetType();

		if (textureType == TextureType::Master)
		{
			return ImGui::ImageButton(reinterpret_cast<void*>(texture->GetRendererID()), size, { 0, 0 }, { 1, 1 }, frame_padding, bg_col, tint_col);
		}
		else if (textureType == TextureType::SubTexture)
		{
			if (auto subTexture = std::dynamic_pointer_cast<SubTexture2D>(texture))
			{
				Rect rect = subTexture->GetRect();

				return ImGui::ImageButton(
					reinterpret_cast<void*>(texture->GetRendererID()), size,
					{ rect.x, rect.y }, { rect.z + rect.x, rect.w + rect.y },
					frame_padding, bg_col, tint_col
				);
			}
		}
	}
}
