#include "pcheader.h"
#include "Serializer.h"

namespace OverEngine
{
	UnorderedMap<String, Serializer::EnumValues> Serializer::s_Enums;

	void Serializer::SerializeToYaml(const SerializationContext& ctx, void* source, YAML::Emitter& out)
	{
		for (const auto& element : ctx.Elements)
		{
			void* item = (char*)source + element.Offset;
			out << YAML::Key << element.Name << YAML::Value;

			switch (element.Type)
			{
			case SerializableType::Float:     out << *(float*)item; break;
			case SerializableType::Float2:    out << *(Vector2*)item; break;
			case SerializableType::Float3:    out << *(Vector3*)item; break;
			case SerializableType::Float4:    out << *(Vector4*)item; break;
			case SerializableType::Int:       out << *(int*)item; break;
			case SerializableType::UInt:      out << *(uint32_t*)item; break;
			case SerializableType::Int8:      out << (int)*(int8_t*)item; break;
			case SerializableType::UInt8:     out << (int)*(uint8_t*)item; break;
			case SerializableType::IntEnum:   out << s_Enums[element.EnumName][*(int*)item]; break;
			case SerializableType::UIntEnum:  out << s_Enums[element.EnumName][(int)*(uint32_t*)item]; break;
			case SerializableType::Int8Enum:  out << s_Enums[element.EnumName][(int)*(int8_t*)item]; break;
			case SerializableType::UInt8Enum: out << s_Enums[element.EnumName][(int)*(uint8_t*)item]; break;
			case SerializableType::Bool:      out << *(bool*)item; break;
			case SerializableType::String:    out << *(String*)item; break;
			default:
				OE_CORE_ASSERT(false, "Unknown SerializableType!");
				out << YAML::Null;
				break;
			}
		}
	}

	void Serializer::DefineGlobalEnum(const String& name, const EnumValues& values)
	{
		OE_CORE_ASSERT(!GlobalEnumExists(name), "Enum '{0}' already exists!", name);
		s_Enums[name] = values;
	}

	bool Serializer::GlobalEnumExists(const String& name)
	{
		return s_Enums.find(name) != s_Enums.end();
	}

	Serializer::EnumValues& Serializer::GetGlobalEnum(const String& name)
	{
		OE_CORE_ASSERT(GlobalEnumExists(name), "Enum '{0}' doesn't exists!", name);
		return s_Enums[name];
	}

	int Serializer::GetGlobalEnumValue(const String& enumName, const String& name)
	{
		OE_CORE_ASSERT(GlobalEnumExists(enumName), "Enum '{0}' doesn't exists!", enumName);

		for (auto value : s_Enums[enumName])
			if (value.second == name)
				return value.first;
		
		OE_CORE_ASSERT(false, "Can't find enum value for {0}", name);
		return INT_MAX;
	}
}
