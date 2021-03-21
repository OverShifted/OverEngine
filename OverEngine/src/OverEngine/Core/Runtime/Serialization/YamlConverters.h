#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Core/Math/Math.h"

#include <yaml-cpp/yaml.h>

namespace YAML
{
	template<>
	struct convert<OverEngine::Vector2>
	{
		static Node encode(const OverEngine::Vector2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, OverEngine::Vector2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<OverEngine::Vector3>
	{
		static Node encode(const OverEngine::Vector3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, OverEngine::Vector3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<OverEngine::Vector4>
	{
		static Node encode(const OverEngine::Vector4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, OverEngine::Vector4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.r = node[0].as<float>();
			rhs.g = node[1].as<float>();
			rhs.b = node[2].as<float>();
			rhs.a = node[3].as<float>();
			return true;
		}
	};
}

namespace OverEngine
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const OverEngine::Vector2& v);
	YAML::Emitter& operator<<(YAML::Emitter& out, const OverEngine::Vector3& v);
	YAML::Emitter& operator<<(YAML::Emitter& out, const OverEngine::Vector4& v);

	class YamlCppHelper
	{
	public:
		static bool IsNodeString(const YAML::Node& node);
	};
}
