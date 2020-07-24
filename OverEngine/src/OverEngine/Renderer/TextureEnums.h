#pragma once

namespace OverEngine
{
	enum class TextureType { Master, Subtexture };
	enum class TextureFiltering { Nearest, Linear };
	enum class TextureWrapping { Repeat = 0, MirroredRepeat, ClampToEdge, ClampToBorder };
	enum class TextureFormat { None, RGB, RGBA };
}