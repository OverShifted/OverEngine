#pragma once

#include "OverEngine/Core/Core.h"

namespace OverEngine
{
	class Texture
	{
	public:
		enum class Filtering { Nearest, Linear };
		enum class Type { None, RGB, RGBA };
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual Filtering GetMinFilter() const = 0;
		virtual Filtering GetMagFilter() const = 0;

		virtual void SetMinFilter(Filtering filter) = 0;
		virtual void SetMagFilter(Filtering filter) = 0;

		virtual Type GetType() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Texture2D* Create(const String& path, Filtering minFilter = Filtering::Linear, Filtering magFilter = Filtering::Linear);
	};
}