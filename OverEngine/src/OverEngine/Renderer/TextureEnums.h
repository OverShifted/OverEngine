#pragma once

namespace OverEngine
{
	enum class TextureType : uint8_t
	{
		None = 0,
		Master,
		Subtexture,
		Placeholder
	};

	enum class TextureFilter : uint8_t
	{
		None = 0,
		Nearest,
		Linear
	};

	enum class TextureWrap  : uint8_t
	{
		None = 0,
		Repeat,
		Clamp,
		Mirror
	};

	enum class TextureFormat : uint8_t
	{
		None = 0,
		RGB8,
		RGBA8
	};

	using TextureFlip = uint8_t;
	enum TextureFlip_ : uint8_t
	{
		TextureFlip_None = 0,
		TextureFlip_X = BIT(0),
		TextureFlip_Y = BIT(1),
		TextureFlip_Both = TextureFlip_X | TextureFlip_Y
	};
}
