#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Core/Math/Math.h"

#include "OverEngine/Core/Runtime/Serialization/YamlConverters.h"
#include <yaml-cpp/yaml.h>

#include <optional>

// Based on: https://github.com/preshing/FlexibleReflection

namespace OverEngine::Reflection
{
	///////////////////////////////////////////////////////////////
	// Base class of all type descriptors /////////////////////////
	///////////////////////////////////////////////////////////////

	struct TypeDescriptor
	{
		const char* Name;
		size_t Size;

		TypeDescriptor(const char* name, size_t size) : Name(name), Size(size) {}
		virtual ~TypeDescriptor() {}
		virtual String GetFullName() const { return Name; }
		virtual void Serialize(const void* obj, YAML::Emitter& out, bool pushMap = true) const = 0;
	};

	///////////////////////////////////////////////////////////////
	// Type descriptors for user-defined enum classes /////////////
	///////////////////////////////////////////////////////////////

	template <typename IntType>
	struct TypeDescriptor_EnumClass : TypeDescriptor
	{
		UnorderedMap<IntType, String> values;

		TypeDescriptor_EnumClass(const char* name, size_t size, const UnorderedMap<IntType, String>& init)
			: TypeDescriptor(name, size), values(init)
		{}

		virtual void Serialize(const void* obj, YAML::Emitter& out, bool pushMap = true) const override
		{
			out << YAML::Value << values.at(*(const IntType *)obj);
		}
	};

	#define OE_REFLECT_ENUM_CLASS_BEGIN(type)                                                                     \
		namespace {                                                                                               \
			static char _ = [] () -> char {                                                                       \
				using T = type;                                                                                   \
				static auto desc = ::OverEngine::Reflection::TypeDescriptor_EnumClass<std::underlying_type_t<T>>( \
					#type, sizeof(type),                                                                          \
					{

	#define OE_REFLECT_ENUM_CLASS_VALUE(name, val) { val, #name },

	#define OE_REFLECT_ENUM_CLASS_END() }); ::OverEngine::Reflection::EnumClassResolver::Register<T>(reinterpret_cast<::OverEngine::Reflection::TypeDescriptor*>(&desc)); return 0; }(); }

	///////////////////////////////////////////////////////////////
	// Finding type descriptors ///////////////////////////////////
	///////////////////////////////////////////////////////////////

	struct EnumClassResolver
	{
		static Scope<UnorderedMap<size_t, TypeDescriptor*>> Descriptors;

		template<typename T>
		static TypeDescriptor_EnumClass<std::underlying_type_t<T>>* Get()
		{
			return reinterpret_cast<TypeDescriptor_EnumClass<std::underlying_type_t<T>>*>(Descriptors->at(typeid(T).hash_code()));
		}

		template<typename T>
		static void Register(TypeDescriptor* desc)
		{
			if (Descriptors == nullptr)
			{
				Descriptors.reset(new UnorderedMap<size_t, TypeDescriptor*>());
			}

			(*Descriptors)[typeid(T).hash_code()] = desc;
		}

		template<typename T>
		static std::optional<T> GetValueByName(const String& name)
		{
			auto desc = Get<T>();
			for (const auto& pair : desc->values)
			{
				if (pair.second == name)
					return (T)pair.first;
			}

			return {};
		}

		template<typename T>
		static const String& GetNameByValue(const T& value)
		{
			return Get<T>()->values.at(static_cast<std::underlying_type_t<T>>(value));
		}
	};

	// A helper class to find TypeDescriptors in different ways:
	struct DefaultResolver
	{
		template <typename T>
		static char func(decltype(&T::_Reflection));

		template <typename T>
		static int func(...);

		template <typename T>
		struct IsReflected
		{
			enum
			{
				value = (sizeof(func<T>(nullptr)) == sizeof(char))
			};
		};

		// This version is called if T has a static member named "_Reflection":
		template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
		static TypeDescriptor* Get()
		{
			return &T::_Reflection;
		}

		// This version is called otherwise:
		template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
		static TypeDescriptor* Get()
		{
			return reinterpret_cast<TypeDescriptor*>(EnumClassResolver::Get<T>());
		}
	};

	// This is the primary class template for finding all TypeDescriptors:
	template <typename T>
	struct TypeResolver
	{
		static TypeDescriptor* Get()
		{
			return DefaultResolver::Get<T>();
		}
	};

	///////////////////////////////////////////////////////////////
	// Type descriptors for user-defined structs/classes //////////
	///////////////////////////////////////////////////////////////

	struct TypeDescriptor_Struct : TypeDescriptor
	{
		struct Member
		{
			const char* Name;
			size_t Offset;
			TypeDescriptor* Type;

#if 0
			enum Flags_ : uint8_t
			{
				Flags_None = 0,
				Flags_HideInInspector = BIT(1)
			};

			uint8_t flags = Flags_None;
#endif
		};

		Vector<Member> Members;

		TypeDescriptor_Struct(void (*init)(TypeDescriptor_Struct*))
			: TypeDescriptor(nullptr, 0)
		{
			init(this);
		}

		TypeDescriptor_Struct(const char* name, size_t size, const std::initializer_list<Member> &init)
			: TypeDescriptor(name, size), Members(init)
		{}

		virtual void Serialize(const void* obj, YAML::Emitter& out, bool pushMap = true) const override
		{
			if (pushMap)
				out << YAML::BeginMap;
			for (const Member& member : Members)
			{
				out << YAML::Key << member.Name;
				member.Type->Serialize((char*)obj + member.Offset, out);
			}
			if (pushMap)
				out << YAML::EndMap;
		}
	};

	#define OE_REFLECT_STRUCT()                                                         \
		friend struct ::OverEngine::Reflection::DefaultResolver;                        \
		static ::OverEngine::Reflection::TypeDescriptor_Struct _Reflection;             \
		static void _InitReflection(::OverEngine::Reflection::TypeDescriptor_Struct*);

	#define OE_REFLECT_STRUCT_BEGIN(type)                                                         \
		::OverEngine::Reflection::TypeDescriptor_Struct type::_Reflection{type::_InitReflection}; \
		void type::_InitReflection(::OverEngine::Reflection::TypeDescriptor_Struct* typeDesc)     \
		{                                                                                         \
			using T = type;                                                                       \
			typeDesc->Name = #type;                                                               \
			typeDesc->Size = sizeof(T);                                                           \
			typeDesc->Members = {

	#define OE_REFLECT_STRUCT_MEMBER(name) { #name, OffsetOf(&T::name), ::OverEngine::Reflection::TypeResolver<decltype(T::name)>::Get() },

	#define OE_REFLECT_STRUCT_END() };}

	///////////////////////////////////////////////////////////////
	// Basic type descriptors /////////////////////////////////////
	///////////////////////////////////////////////////////////////

	#define BASIC_INT_PRIMITIVE(T, TNameGlue)                                                               \
		struct TypeDescriptor_##TNameGlue : TypeDescriptor                                                  \
		{                                                                                                   \
			TypeDescriptor_##TNameGlue()                                                                    \
				: TypeDescriptor(#T, sizeof(T))                                                             \
			{}                                                                                              \
																											\
			virtual void Serialize(const void* obj, YAML::Emitter& out, bool pushMap = true) const override \
			{                                                                                               \
				out << YAML::Value << +*(const T*)obj;                                                      \
			}                                                                                               \
		};                                                                                                  \
																											\
		template <>                                                                                         \
		class TypeResolver<T>                                                                               \
		{                                                                                                   \
		public:                                                                                             \
			static TypeDescriptor* Get()                                                                    \
			{                                                                                               \
				static TypeDescriptor_##TNameGlue typeDesc;                                                 \
				return &typeDesc;                                                                           \
			}                                                                                               \
		};

	#define BASIC_NON_INT_PRIMITIVE(T, TNameGlue)                                                           \
		struct TypeDescriptor_##TNameGlue : TypeDescriptor                                                  \
		{                                                                                                   \
			TypeDescriptor_##TNameGlue()                                                                    \
				: TypeDescriptor(#T, sizeof(T))                                                             \
			{}                                                                                              \
																											\
			virtual void Serialize(const void* obj, YAML::Emitter& out, bool pushMap = true) const override \
			{                                                                                               \
				out << YAML::Value << *(const T*)obj;                                                       \
			}                                                                                               \
		};                                                                                                  \
																											\
		template <>                                                                                         \
		class TypeResolver<T>                                                                               \
		{                                                                                                   \
		public:                                                                                             \
			static TypeDescriptor* Get()                                                                    \
			{                                                                                               \
				static TypeDescriptor_##TNameGlue typeDesc;                                                 \
				return &typeDesc;                                                                           \
			}                                                                                               \
		};

	BASIC_INT_PRIMITIVE(int8_t, Int8)
	BASIC_INT_PRIMITIVE(int16_t, Int16)
	BASIC_INT_PRIMITIVE(int32_t, Int32)
	BASIC_INT_PRIMITIVE(int64_t, Int64)

	BASIC_INT_PRIMITIVE(uint8_t, UInt8)
	BASIC_INT_PRIMITIVE(uint16_t, UInt16)
	BASIC_INT_PRIMITIVE(uint32_t, UInt32)
	BASIC_INT_PRIMITIVE(uint64_t, UInt64)

	BASIC_INT_PRIMITIVE(float, Float)
	BASIC_INT_PRIMITIVE(double, Double)

	BASIC_NON_INT_PRIMITIVE(bool, Bool)
	BASIC_NON_INT_PRIMITIVE(String, String)

	#define VECTOR_PRIMITIVE(N)                                                                             \
		struct TypeDescriptor_Vector##N : TypeDescriptor                                                    \
		{                                                                                                   \
			TypeDescriptor_Vector##N()                                                                      \
				: TypeDescriptor("Vector" #N, sizeof(::OverEngine::Vector##N))                              \
			{}                                                                                              \
																											\
			virtual void Serialize(const void* obj, YAML::Emitter& out, bool pushMap = true) const override \
			{                                                                                               \
				out << YAML::Value << *(const ::OverEngine::Vector##N *)obj;                                \
			}                                                                                               \
		};                                                                                                  \
																											\
		template <>                                                                                         \
		class TypeResolver<::OverEngine::Vector##N>                                                         \
		{                                                                                                   \
		public:                                                                                             \
			static TypeDescriptor* Get()                                                                    \
			{                                                                                               \
				static TypeDescriptor_Vector##N typeDesc;                                                   \
				return &typeDesc;                                                                           \
			}                                                                                               \
		};

	VECTOR_PRIMITIVE(2)
	VECTOR_PRIMITIVE(3)
	VECTOR_PRIMITIVE(4)

	///////////////////////////////////////////////////////////////
	// Type descriptors for Vector ///////////////////////////
	///////////////////////////////////////////////////////////////

	struct TypeDescriptor_StdVector : TypeDescriptor
	{
		TypeDescriptor *ItemType;
		size_t (*GetSize)(const void*);
		const void* (*GetItem)(const void*, size_t);
		void (*SerializeToYaml)(const void*, YAML::Emitter&);

		template <typename ItemType>
		TypeDescriptor_StdVector(ItemType*)
			: TypeDescriptor("Vector<>", sizeof(Vector<ItemType>)),
			  ItemType(TypeResolver<ItemType>::get())
		{
			GetSize = [](const void* vecPtr) -> size_t {
				const auto &vec = *(const Vector<ItemType> *)vecPtr;
				return vec.size();
			};

			GetItem = [](const void* vecPtr, size_t index) -> const void* {
				const auto &vec = *(const Vector<ItemType> *)vecPtr;
				return &vec[index];
			};

			SerializeToYaml = [](const void* vecPtr, YAML::Emitter out) -> void {
				const auto &vec = *(const Vector<ItemType> *)vecPtr;
				out << YAML::Value << vec;
			};
		}

		virtual String GetFullName() const override
		{
			return String("Vector<") + ItemType->GetFullName() + ">";
		}

		virtual void Serialize(const void* obj, YAML::Emitter& out, bool pushMap = true) const override 
		{
			SerializeToYaml(obj, out);
		}
	};

	// Partially specialize TypeResolver<> for Vectors:
	template <typename T>
	class TypeResolver<Vector<T>>
	{
	public:
		static TypeDescriptor* Get()
		{
			static TypeDescriptor_StdVector typeDesc{ (T *)nullptr };
			return &typeDesc;
		}
	};
}
