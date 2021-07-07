#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Core/Runtime/Reflection/TypeInfo.h"

#define OE_CLASS_PUBLIC(className, parentClassName)                                                          \
	public:                                                                                                  \
		static constexpr const char* GetStaticClassName() { return #className; }                             \
		virtual const char* GetClassName() const override { return GetStaticClassName(); }                   \
                                                                                                             \
		static constexpr const char* GetStaticParentClassName() { return #parentClassName; }                 \
		virtual const char* GetParentClassName() const override { return GetStaticParentClassName(); }       \
                                                                                                             \
		static ObjectTypeInfo* Reflect();                                                                    \
		virtual ObjectTypeInfo* DynamicReflect() const override { return Reflect(); }

#define OE_CLASS(className, parentClassName)        \
		OE_CLASS_PUBLIC(className, parentClassName) \
	private:

#define OE_CLASS_NO_REFLECT_PUBLIC(className, parentClassName)                                                                                                                                                    \
	public:                                                                                                                                                                                                       \
		static constexpr const char* GetStaticClassName() { return #className; }                                                                                                                                  \
		virtual const char* GetClassName() const override { return GetStaticClassName(); }                                                                                                                        \
                                                                                                                                                                                                                  \
		static constexpr const char* GetStaticParentClassName() { return #parentClassName; }                                                                                                                      \
		virtual const char* GetParentClassName() const override { return GetStaticParentClassName(); }                                                                                                            \
                                                                                                                                                                                                                  \
		static ObjectTypeInfo* Reflect() { static ObjectTypeInfo typeInfo = ObjectTypeInfo(GetStaticClassName(), sizeof(className), (ObjectTypeInfo*)TypeResolver<parentClassName>::Get()); return &typeInfo; }   \
		virtual ObjectTypeInfo* DynamicReflect() const override { return Reflect(); }

#define OE_CLASS_NO_REFLECT(className, parentClassName)        \
		OE_CLASS_NO_REFLECT_PUBLIC(className, parentClassName) \
	private:

namespace OverEngine
{
	struct ObjectTypeInfo;
	class Variant;

	class Object
	{
	public:
		static constexpr const char* GetStaticClassName() { return "Object"; }
		virtual const char* GetClassName() const { return GetStaticClassName(); }

		static constexpr const char* GetStaticParentClassName() { return ""; }
		virtual const char* GetParentClassName() const { return GetStaticParentClassName(); }

		static ObjectTypeInfo* Reflect();
		virtual ObjectTypeInfo* DynamicReflect() const { return Reflect(); }

		virtual bool GetProperty(const String& name, Variant* out);
		virtual bool SetProperty(const String& name, const Variant& value);
	};
}
