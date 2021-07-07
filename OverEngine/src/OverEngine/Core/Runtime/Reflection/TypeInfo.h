#pragma once

#include "OverEngine/Core/Variant.h"

#define ADD_PUBLIC_MEMBER_PROPERTY(klass, member)                                                                                                                                                  \
    {                                                                                                                                                                                              \
		PropertyInfo* property_info_##klass_##member = new PropertyInfo{ #member, #member, TypeResolver<PointerToMemberDecomposer<decltype(&klass::member)>::MemberType>::Get(), PropertyHint(),   \
			[](Object* obj) -> Variant { return dynamic_cast<klass*>(obj)->member; },                                                                                                              \
			[](Object* obj, const Variant& value) { dynamic_cast<klass*>(obj)->member = value; }                                                                                                   \
		};                                                                                                                                                                                         \
		typeInfo->PropertiesOrder.push_back(property_info_##klass_##member);                                                                                                                       \
		typeInfo->Properties[#member] = property_info_##klass_##member;                                                                                                                            \
    }


#define ADD_PUBLIC_MEMBER_ENUM_PROPERTY(klass, member, enumHint)                                                                                                                                             \
	{                                                                                                                                                                                                        \
		PropertyInfo* property_info_##klass_##member = new PropertyInfo{ #member, #member, TypeResolver<EnumType<PointerToMemberDecomposer<decltype(&klass::member)>::MemberType>::Type>::Get(), enumHint,   \
			[](Object* obj) -> Variant { return (int)dynamic_cast<klass*>(obj)->member; },                                                                                                                   \
			[](Object* obj, const Variant& value) { dynamic_cast<klass*>(obj)->member = (PointerToMemberDecomposer<decltype(&klass::member)>::MemberType)(int)value; }                                       \
		};                                                                                                                                                                                                   \
		typeInfo->PropertiesOrder.push_back(property_info_##klass_##member);                                                                                                                                 \
		typeInfo->Properties[#member] = property_info_##klass_##member;                                                                                                                                      \
	}

#define ADD_GET_SET_PROPERTY_INSPECTOR_NAME(klass, name, inspectorName, getter, setter)                                                                                                                                     \
    {                                                                                                                                                                                                                       \
		PropertyInfo* property_info_##klass_##member = new PropertyInfo{ #name, #inspectorName, TypeResolver<std::remove_const_t<std::remove_reference_t<decltype(((klass*)nullptr)->getter())>>>::Get(), PropertyHint(),   \
			[](Object* obj) -> Variant { return dynamic_cast<klass*>(obj)->getter(); },                                                                                                                                     \
			[](Object* obj, const Variant& value) { dynamic_cast<klass*>(obj)->setter(value); }                                                                                                                             \
		};                                                                                                                                                                                                                  \
		typeInfo->PropertiesOrder.push_back(property_info_##klass_##member);                                                                                                                                                \
		typeInfo->Properties[#name] = property_info_##klass_##member;                                                                                                                                                       \
    }

#define ADD_GET_SET_PROPERTY(klass, name, getter, setter) ADD_GET_SET_PROPERTY_INSPECTOR_NAME(klass, name, name, getter, setter)

#define ADD_GET_SET_PROPERTY_INSPECTOR_NAME_READ_ONLY(klass, name, inspectorName, getter)                                                                                                                                   \
    {                                                                                                                                                                                                                       \
		PropertyInfo* property_info_##klass_##member = new PropertyInfo{ #name, #inspectorName, TypeResolver<std::remove_const_t<std::remove_reference_t<decltype(((klass*)nullptr)->getter())>>>::Get(), PropertyHint(),   \
			[](Object* obj) -> Variant { return dynamic_cast<klass*>(obj)->getter(); },                                                                                                                                     \
		};                                                                                                                                                                                                                  \
		typeInfo->PropertiesOrder.push_back(property_info_##klass_##member);                                                                                                                                                \
		typeInfo->Properties[#name] = property_info_##klass_##member;                                                                                                                                                       \
    }

#define ADD_GET_SET_PROPERTY_READ_ONLY(klass, name, getter) ADD_GET_SET_PROPERTY_INSPECTOR_NAME_READ_ONLY(klass, name, name, getter)

namespace OverEngine
{
	// TODO: Range, Multiline text, File path, etc.
	enum class PropertyHintType
	{
		None = 0,
		Enum,
		Flags
	};

	struct PropertyHint
	{
		PropertyHintType Type = PropertyHintType::None;
		Map<int, String> EnumValues;

		PropertyHint() = default;
		PropertyHint(PropertyHintType type, Map<int, String> enumValues)
			: Type(type), EnumValues(enumValues)
		{
		}
	};

	struct TypeInfo
	{
		TypeInfo(const String& name, size_t size)
				: Name(name), Size(size)
		{
		}

		virtual void DrawInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const PropertyHint& hint) {}

		String Name;
		size_t Size;
	};

	template<typename T>
	struct TypeResolver
	{
		static TypeInfo* Get()
		{
			return T::Reflect();
		}
	};

	#define PrimitiveTypeInfo(T)                                                                                                                                                    \
		struct PrimitiveTypeInfo_##T : public TypeInfo                                                                                                                              \
		{                                                                                                                                                                           \
			PrimitiveTypeInfo_##T()                                                                                                                                                 \
				: TypeInfo(#T, sizeof(T))                                                                                                                                           \
			{                                                                                                                                                                       \
			}                                                                                                                                                                       \
																																													\
			virtual void DrawInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const PropertyHint& hint) override;   \
		};                                                                                                                                                                          \
																																													\
		template<>                                                                                                                                                                  \
		struct TypeResolver<T>                                                                                                                                                      \
		{                                                                                                                                                                           \
			static TypeInfo* Get()                                                                                                                                                  \
			{                                                                                                                                                                       \
				static PrimitiveTypeInfo_##T typeInfo;                                                                                                                              \
				return &typeInfo;                                                                                                                                                   \
			}                                                                                                                                                                       \
		};

	PrimitiveTypeInfo(int8_t)
	PrimitiveTypeInfo(int16_t)
	PrimitiveTypeInfo(int32_t)
	PrimitiveTypeInfo(int64_t)

	PrimitiveTypeInfo(uint8_t)
	PrimitiveTypeInfo(uint16_t)
	PrimitiveTypeInfo(uint32_t)
	PrimitiveTypeInfo(uint64_t)

	PrimitiveTypeInfo(bool)

	PrimitiveTypeInfo(float)
	PrimitiveTypeInfo(double)

	PrimitiveTypeInfo(Vector2)
	PrimitiveTypeInfo(Vector3)
	PrimitiveTypeInfo(Vector4)

	PrimitiveTypeInfo(String)

	struct RefTypeInfo : public TypeInfo
	{
		RefTypeInfo(TypeInfo* targetType)
			: TypeInfo("Ref", sizeof(Ref<void>)), TargetType(targetType)
		{
		}

		virtual void DrawInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const PropertyHint& hint) override;

		TypeInfo* TargetType;
	};

	template<typename T>
	struct TypeResolver<Ref<T>>
	{
		static TypeInfo* Get()
		{
			static RefTypeInfo typeInfo{ TypeResolver<T>::Get() };
			return &typeInfo;
		}
	};

	struct PropertyInfo
	{
		String Name;
		String InspectorName;

		TypeInfo* Type = nullptr;
		PropertyHint Hint;

		PropertyGetter Getter = nullptr;
		PropertySetter Setter = nullptr;
	};

	struct ObjectTypeInfo : public TypeInfo
	{
		ObjectTypeInfo(const String& name, size_t size, ObjectTypeInfo* parentClass = nullptr)
			: TypeInfo(name, size), ParentClass(parentClass)
		{
		}

		ObjectTypeInfo* ParentClass = nullptr;
		UnorderedMap<String, PropertyInfo*> Properties;
		Vector<PropertyInfo*> PropertiesOrder;
	};
}
