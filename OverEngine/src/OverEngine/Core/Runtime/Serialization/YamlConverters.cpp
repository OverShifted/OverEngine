#include "pcheader.h"
#include "YamlConverters.h"

namespace OverEngine
{
	bool YamlDecoder<Vector2>::Decode(const YAML::Node& node, Vector2& rhs)
	{
		if (!node.IsSequence() || node.size() != 2)
			return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		return true;
	}

	bool YamlDecoder<Vector3>::Decode(const YAML::Node& node, Vector3& rhs)
	{
		if (!node.IsSequence() || node.size() != 3)
			return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		return true;
	}

	bool YamlDecoder<Vector4>::Decode(const YAML::Node& node, Vector4& rhs)
	{
		if (!node.IsSequence() || node.size() != 4)
			return false;

		rhs.r = node[0].as<float>();
		rhs.g = node[1].as<float>();
		rhs.b = node[2].as<float>();
		rhs.a = node[3].as<float>();
		return true;
	}

	void YamlEmitter<Vector2>::Emit(YAML::Emitter& out, const Vector2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
	}

	void YamlEmitter<Vector3>::Emit(YAML::Emitter& out, const Vector3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	}

	void YamlEmitter<Vector4>::Emit(YAML::Emitter& out, const Vector4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	}
}
