#pragma once

#include "Components.h"
#include "OverEngine/Core/Runtime/Serialization/YamlConverters.h"

#include <yaml-cpp/yaml.h>

namespace OverEngine
{
	template<>
	class YamlEmitter<NameComponent>
	{
	public:
		static void Emit(YAML::Emitter& out, const NameComponent& nc)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Name" << YAML::Value << nc.Name;
			out << YAML::EndMap;
		}
	};

	template<>
	class YamlEmitter<TransformComponent>
	{
	public:
		static void Emit(YAML::Emitter& out, const TransformComponent& tc)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Parent" << YAML::Value;
			if (auto parent = tc.GetParent())
			{
				out << YAML::Hex << parent.GetComponent<IDComponent>().ID;
				out << YAML::Key << "SiblingIndex" << YAML::Value << tc.GetSiblingIndex();
			}
			else
			{
				out << YAML::Null;
			}

			out << YAML::Key << "Position" << YAML::Value << tc.GetLocalPosition();
			out << YAML::Key << "Rotation" << YAML::Value << tc.GetLocalEulerAngles();
			out << YAML::Key << "Scale" << YAML::Value << tc.GetLocalScale();
			out << YAML::EndMap;
		}
	};

	template<>
	class YamlEmitter<CameraComponent>
	{
	public:
		static void Emit(YAML::Emitter& out, const CameraComponent& cc)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)cc.Camera.GetProjectionType();

			out << YAML::Key << "PerspectiveFOV" << YAML::Value << cc.Camera.GetPerspectiveFOV();
			out << YAML::Key << "PerspectiveNearClip" << YAML::Value << cc.Camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFarClip" << YAML::Value << cc.Camera.GetPerspectiveFarClip();

			out << YAML::Key << "OrthographicSize" << YAML::Value << cc.Camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNearClip" << YAML::Value << cc.Camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFarClip" << YAML::Value << cc.Camera.GetOrthographicFarClip();

			out << YAML::Key << "ClearFlags" << YAML::Value << (int)cc.Camera.GetClearFlags();
			out << YAML::Key << "ClearColor" << YAML::Value << cc.Camera.GetClearColor();

			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cc.FixedAspectRatio;
			out << YAML::EndMap;
		}
	};

	template<>
	class YamlDecoder<CameraComponent>
	{
	public:
		static bool Decode(const YAML::Node& node, CameraComponent& cc)
		{
			auto cameraProps = node["Camera"];
			cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

			cc.Camera.SetPerspectiveFOV(cameraProps["PerspectiveFOV"].as<float>());
			cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
			cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

			cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
			cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
			cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

			cc.Camera.SetClearFlags(cameraProps["ClearFlags"].as<uint8_t>());
			cc.Camera.SetClearColor(cameraProps["ClearColor"].as<Color>());

			cc.FixedAspectRatio = node["FixedAspectRatio"].as<bool>();

			return true;
		}
	};

	template<>
	class YamlEmitter<SpriteRendererComponent>
	{
	public:
		static void Emit(YAML::Emitter& out, const SpriteRendererComponent& sp)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Tint" << YAML::Value << sp.Tint;

			out << YAML::Key << "Sprite" << YAML::Value;
			// if (sp.Sprite)
			// {
			// 	out << YAML::Flow << YAML::BeginMap;
			// 	out << YAML::Key << "Asset" << YAML::Value << YAML::Hex << sp.Sprite->GetGuid();
			// 	out << YAML::EndMap;
			// }
			// else
			{
				out << YAML::Null;
			}

			out << YAML::Key << "Tiling" << YAML::Value << sp.Tiling;
			out << YAML::Key << "Offset" << YAML::Value << sp.Offset;
			out << YAML::Key << "Flip" << YAML::Value << (int)sp.Flip;

			out << YAML::Key << "ForceTile" << YAML::Value << sp.ForceTile;
			out << YAML::EndMap;
		}
	};

	template<>
	class YamlDecoder<SpriteRendererComponent>
	{
	public:
		static bool Decode(const YAML::Node& node, SpriteRendererComponent& sp)
		{
			if (!node["Sprite"].IsNull())
			{
				uint32_t refID = node["Sprite"]["Reference"].as<uint32_t>();
				auto asset = node["References"][refID];

				String typeName = asset["Type"].as<String>();
				// if (typeName == Texture2D::GetStaticClassName())
				// 	sp.Sprite = AssetDatabase::RegisterAndGet<Texture2D>(asset["Guid"].as<uint64_t>());
			}

			sp.Tint = node["Tint"].as<Color>();
			sp.Tiling = node["Tiling"].as<Vector2>();

			if (node["Flip.x"].as<bool>())
				sp.Flip |= TextureFlip_X;
			if (node["Flip.y"].as<bool>())
				sp.Flip |= TextureFlip_Y;

			return true;
		}
	};

	template<>
	class YamlEmitter<RigidBody2DComponent>
	{
	public:
		static void Emit(YAML::Emitter& out, const RigidBody2DComponent& rbc)
		{
			out << YAML::BeginMap;
			auto& props = rbc.RigidBody->GetProps();
			out << YAML::Key << "Type" << YAML::Value << (int)props.Type;
			out << YAML::Key << "LinearVelocity" << YAML::Value << props.Dynamics.LinearVelocity;
			out << YAML::Key << "AngularVelocity" << YAML::Value << props.Dynamics.AngularVelocity;
			out << YAML::Key << "IsAwake" << YAML::Value << props.Dynamics.IsAwake;
			out << YAML::Key << "LinearDamping" << YAML::Value << props.LinearDamping;
			out << YAML::Key << "AngularDamping" << YAML::Value << props.AngularDamping;
			out << YAML::Key << "AllowSleep" << YAML::Value << props.AllowSleep;
			out << YAML::Key << "FixedRotation" << YAML::Value << props.FixedRotation;
			out << YAML::Key << "GravityScale" << YAML::Value << props.GravityScale;
			out << YAML::Key << "Bullet" << YAML::Value << props.Bullet;
			out << YAML::EndMap;
		}
	};

	template<>
	class YamlDecoder<RigidBody2DComponent>
	{
	public:
		static bool Decode(const YAML::Node& node, RigidBody2DComponent& rbc)
		{
			RigidBody2DProps props;
			props.Type                     = (RigidBody2DType)node["Type"].as<int>();
			props.Dynamics.LinearVelocity  = node["LinearVelocity"].as<Vector2>();
			props.Dynamics.AngularVelocity = node["AngularVelocity"].as<float>();
			props.LinearDamping            = node["LinearDamping"].as<float>();
			props.AngularDamping           = node["AngularDamping"].as<float>();
			props.AllowSleep               = node["AllowSleep"].as<bool>();
			props.Dynamics.IsAwake         = node["Awake"].as<bool>();
			props.FixedRotation            = node["FixedRotation"].as<bool>();
			props.GravityScale             = node["GravityScale"].as<float>();
			props.Bullet                   = node["Bullet"].as<bool>();
			rbc.RigidBody                  = RigidBody2D::Create(props);

			return true;
		}
	};
}
