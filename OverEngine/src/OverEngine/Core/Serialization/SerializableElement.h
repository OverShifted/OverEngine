#pragma once

#define SERIALIZE_FIELD(type, field) &type::field, #field

namespace OverEngine
{
	enum class SerializableType
	{
		None = 0,
		// Float components
		Float, Float2, Float3, Float4,
		// Int components
		Int, UInt, Int8, UInt8,
		IntEnum, UIntEnum, Int8Enum, UInt8Enum,
		// Other
		Bool, String
	};

	struct SerializableElement
	{
		String Name = String();
		SerializableType Type = SerializableType::None;
		uint32_t Offset = 0;

		String EnumName = String();

		SerializableElement() = default;

		SerializableElement(SerializableType type, const String& name = String())
			: Name(name), Type(type) {}
	};

	struct SerializationContext
	{
		Vector<SerializableElement> Elements;

		SerializationContext() = default;

		template<typename T, typename U>
		void AddField(SerializableType type, U T::* member, const String& name = String())
		{
			AddField(type, OffsetOf(member), name);
		}

		void AddField(SerializableType type, uint32_t offset, const String& name = String())
		{
			SerializableElement elem(type, name);
			elem.Offset = offset;
			Elements.push_back(elem);
		}

		template<typename T, typename U>
		void AddEnumField(SerializableType type, const String& enumName, U T::* member, const String& name = String())
		{
			AddEnumField(type, enumName, OffsetOf(member), name);
		}

		void AddEnumField(SerializableType type, const String& enumName, uint32_t offset, const String& name = String())
		{
			SerializableElement elem(type, name);
			elem.Offset = offset;
			elem.EnumName = enumName;
			Elements.push_back(elem);
		}

		const SerializableElement& operator[] (const char* name) const
		{
			for (const auto& element : Elements)
				if (element.Name == name)
					return element;

			OE_CORE_ASSERT(false, "Cannot find corresponding element");
			return NULL_REF(SerializableElement);
		}
	};
}
