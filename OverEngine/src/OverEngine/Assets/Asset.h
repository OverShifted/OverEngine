#pragma once

#include "OverEngine/Core/Core.h"

namespace OverEngine
{
	class Asset
	{
	public:
		enum class Type
		{
			Texture, Shader
		};
	private:
		Type m_Type;
	};
}