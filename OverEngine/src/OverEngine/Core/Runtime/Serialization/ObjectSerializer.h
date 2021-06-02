#pragma once

#include "YamlConverters.h"
#include <yaml-cpp/yaml.h>

namespace OverEngine
{
	template<typename T>
	class ObjectSerializer
	{
	public:
		static bool Serialize(YAML::Emitter& out, const T* object) { return false; };
		static bool Deserialize(YAML::Node data, T* object) { return false; };
	};
}
