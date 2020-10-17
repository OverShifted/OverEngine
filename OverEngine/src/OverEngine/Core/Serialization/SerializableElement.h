#pragma once

#define SERIALIZE_FIELD(type, field) &type::field, #field

namespace OverEngine
{
	enum class SerializableDataType
	{
		None = 0,
		// Float components
		Float, Float2, Float3, Float4, Mat3, Mat4,
		// Int components
		Int, UInt, Int8, UInt8,
		IntEnum, UIntEnum, Int8Enum, UInt8Enum,
		// Other
		Bool, String
	};

	struct SerializableElement
	{
		String Name = String();
		SerializableDataType Type = SerializableDataType::None;
		uint32_t Offset = 0;

		String EnumName = String();

		SerializableElement() = default;

		SerializableElement(SerializableDataType type, const String& name = String())
			: Name(name), Type(type) {}
	};

	struct SerializationContext
	{
		Vector<SerializableElement> Elements;

		SerializationContext() = default;

		template<typename T, typename U>
		void AddField(SerializableDataType type, U T::* member, const String& name = String())
		{
			AddField(type, OffsetOf(member), name);
		}

		void AddField(SerializableDataType type, uint32_t offset, const String& name = String())
		{
			SerializableElement elem(type, name);
			elem.Offset = offset;
			Elements.push_back(elem);
		}

		template<typename T, typename U>
		void AddEnumField(SerializableDataType type, const String& enumName, U T::* member, const String& name = String())
		{
			AddEnumField(type, enumName, OffsetOf(member), name);
		}

		void AddEnumField(SerializableDataType type, const String& enumName, uint32_t offset, const String& name = String())
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
