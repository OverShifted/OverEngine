#pragma once

#include "OverEngine/Core/Core.h"
#include <yaml-cpp/yaml.h>

namespace OverEngine
{
	template <typename T>
	class YamlEmitter;

	#define OE_DECL_EMITTER(T)                                   \
		template<>                                            \
		struct YamlEmitter<T>                                 \
		{                                                     \
			static void Emit(YAML::Emitter& out, const T& v); \
		};

	template <typename T>
	class YamlDecoder;

	#define OE_DECL_DECODER(T)                                     \
		template<>                                              \
		struct YamlDecoder<T>                                   \
		{                                                       \
			static bool Decode(const YAML::Node& node, T& rhs); \
		};
}

namespace YAML
{
	template <typename T>
	YAML::Emitter& operator<<(YAML::Emitter& out, const T& v)
	{
		::OverEngine::YamlEmitter<T>::Emit(out, v);
		return out;
	}

	template <typename T>
	struct convert<T, 0>
	{
	public:
		static bool decode(const Node& node, T& rhs)
		{
			return ::OverEngine::YamlDecoder<T>::Decode(node, rhs);
		}
	};
}
