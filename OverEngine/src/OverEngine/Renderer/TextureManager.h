#pragma once

#include "OverEngine/Renderer/Texture.h"

namespace OverEngine
{
	class TextureManager
	{
	public:
		static void Init();
		static void Shutdown();

		static void AddTexture(Ref<Texture2D>& texture);
	};
}
