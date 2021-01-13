#pragma once

#include "SerializableElement.h"

#include "OverEngine/Core/Serialization/YamlConverters.h"
#include <yaml-cpp/yaml.h>

namespace OverEngine
{
	class Serializer
	{
	public:
		static void SerializeToYaml(const SerializationContext& ctx, void* source, YAML::Emitter& out);

		using EnumValues = UnorderedMap<int, String>;
		static void DefineGlobalEnum(const String& name, const EnumValues& values);
		static bool GlobalEnumExists(const String& name);
		static EnumValues& GetGlobalEnum(const String& name);
		static int GetGlobalEnumValue(const String& enumName, const String& name);
	private:
		static UnorderedMap<String, EnumValues> s_Enums;
	};
}
