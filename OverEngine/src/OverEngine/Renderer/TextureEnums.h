#pragma once

namespace OverEngine
{
	enum class TextureType      : int8_t { Master = 0, Subtexture, Placeholder };
	enum class TextureFiltering : int8_t { None   = 0, Nearest, Linear };
	enum class TextureWrapping  : int8_t
	{
		None = 0, Repeat, Clamp, Mirror,
		ClampToBorder // Only used for GPUTextures
	};
	enum class TextureFormat    : int8_t { None   = 0, RGB, RGBA };
}
