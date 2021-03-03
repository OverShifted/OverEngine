#pragma once

#include "OverEngine/Core/Core.h"

#define OE_CLASS(className)                                                                   \
	public:                                                                                   \
		static constexpr const char* GetStaticClassName() { return #className; }              \
		virtual const char* GetClassName() const override { return GetStaticClassName(); }    \
	private:

namespace OverEngine
{
	class Object
	{
	public:
		static constexpr const char* GetStaticClassName() { return "Object"; }
		virtual const char* GetClassName() const { return GetStaticClassName(); }
	};
}
