#pragma once

namespace OverEngine
{
	enum class TextureType { Master = 0, Subtexture };
	enum class TextureFiltering { None = 0, Nearest, Linear };
	enum class TextureWrapping { None = 0, Repeat, MirroredRepeat, ClampToEdge, ClampToBorder };
	enum class TextureFormat { None = 0, RGB, RGBA };
}
