#pragma once

#include "OverEngine/Core/Runtime/Reflection/Reflection.h"

#include <yaml-cpp/yaml.h>

namespace OverEngine
{
	class Serializer
	{
	public:
		template<typename T>
		static void SerializeToYaml(const T* source, YAML::Emitter& out, bool pushMap = true)
		{
			Reflection::TypeResolver<T>::Get()->Serialize(source, out, pushMap);
		}
	};
}
