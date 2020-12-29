#pragma once

#include "OverEngine/Renderer/Texture.h"

namespace OverEngine
{
	class TextureManager
	{
	public:
		static void Init();
		static void Shutdown();

		static void AddTexture(const Ref<Texture2D>& texture);
		static void RemoveTexture(Texture2D* texture);
		static void ReloadTexture(Texture2D* texture);
	};
}
