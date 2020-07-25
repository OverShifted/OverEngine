#pragma once

namespace OverEngine
{
	enum class TextureType { Master = 0, Subtexture };
	enum class TextureFiltering { Nearest = 0, Linear };
	enum class TextureWrapping { None = -1, Repeat = 0, MirroredRepeat, ClampToEdge, ClampToBorder };
	enum class TextureFormat { None = 0, RGB, RGBA };
}