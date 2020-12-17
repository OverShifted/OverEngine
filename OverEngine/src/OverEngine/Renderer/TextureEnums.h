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
	enum class TextureFormat : int8_t { None   = 0, RGB, RGBA };
	enum class TextureOptimization : int8_t
	{
		None = 0, // Texture allocates its own GPU texture object
		Batched, // used for Renderer2D and GUI
	};
}
