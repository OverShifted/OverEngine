#pragma once

#include "Components.h"
#include "OverEngine/Core/Runtime/Serialization/ObjectSerializer.h"
#include "OverEngine/Core/AssetManagement/AssetDatabase.h"

namespace OverEngine
{
	template<>
	class ObjectSerializer<TransformComponent>
	{
	public:
		static bool Serialize(YAML::Emitter& out, const TransformComponent* object)
		{
			const TransformComponent& tc = *object;

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

			return true;
		};
	};

	template<>
	class ObjectSerializer<CameraComponent>
	{
	public:
		static bool Serialize(YAML::Emitter& out, const CameraComponent* object)
		{
			const CameraComponent& cc = *object;

			out << YAML::Key << "Enabled" << YAML::Value << cc.Enabled;
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

			return true;
		};

		static bool Deserialize(YAML::Node data, CameraComponent* object)
		{
			CameraComponent& cc = *object;

			auto cameraProps = data["Camera"];
			cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

			cc.Camera.SetPerspectiveFOV(cameraProps["PerspectiveFOV"].as<float>());
			cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
			cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

			cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
			cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
			cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

			cc.Camera.SetClearFlags(cameraProps["ClearFlags"].as<uint8_t>());
			cc.Camera.SetClearColor(cameraProps["ClearColor"].as<Color>());

			cc.FixedAspectRatio = data["FixedAspectRatio"].as<bool>();

			return true;
		};
	};

	template<>
	class ObjectSerializer<SpriteRendererComponent>
	{
	public:
		static bool Serialize(YAML::Emitter& out, const SpriteRendererComponent* object)
		{
			const SpriteRendererComponent& sp = *object;

			out << YAML::Key << "Enabled" << YAML::Value << sp.Enabled;

			out << YAML::Key << "Tint" << YAML::Value << sp.Tint;

			out << YAML::Key << "Sprite" << YAML::Value;
			if (sp.Sprite)
			{
				out << YAML::Flow << YAML::BeginMap;
				out << YAML::Key << "Asset" << YAML::Value << YAML::Hex << sp.Sprite->GetGuid();
				out << YAML::EndMap;
			}
			else
			{
				out << YAML::Null;
			}

			out << YAML::Key << "Tiling" << YAML::Value << sp.Tiling;
			out << YAML::Key << "Offset" << YAML::Value << sp.Offset;
			out << YAML::Key << "Flip" << YAML::Value << (int)sp.Flip;

			out << YAML::Key << "ForceTile" << YAML::Value << sp.ForceTile;

			return true;
		};

		static bool Deserialize(YAML::Node data, SpriteRendererComponent* object)
		{
			SpriteRendererComponent& sp = *object;

			if (!data["Sprite"].IsNull())
			{
				uint32_t refID = data["Sprite"]["Reference"].as<uint32_t>();
				auto asset = data["References"][refID];

				String typeName = asset["Type"].as<String>();
				if (typeName == Texture2D::GetStaticClassName())
					sp.Sprite = AssetDatabase::RegisterAndGet<Texture2D>(asset["Guid"].as<uint64_t>());
			}

			sp.Tint = data["Tint"].as<Color>();
			sp.Tiling = data["Tiling"].as<Vector2>();

			if (data["Flip.x"].as<bool>())
				sp.Flip |= TextureFlip_X;
			if (data["Flip.y"].as<bool>())
				sp.Flip |= TextureFlip_Y;

			return true;
		}
	};

	template<>
	class ObjectSerializer<RigidBody2DComponent>
	{
	public:
		static bool Serialize(YAML::Emitter& out, const RigidBody2DComponent* object)
		{
			const RigidBody2DComponent& rbc = *object;

			out << YAML::Key << "Enabled" << YAML::Value << rbc.Enabled;

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

			return true;
		};

		static bool Deserialize(YAML::Node data, RigidBody2DComponent* object)
		{
			RigidBody2DComponent& rbc = *object;

			RigidBody2DProps props;
			props.Type                     = (RigidBody2DType)data["Type"].as<int>();
			props.Dynamics.LinearVelocity  = data["LinearVelocity"].as<Vector2>();
			props.Dynamics.AngularVelocity = data["AngularVelocity"].as<float>();
			props.LinearDamping            = data["LinearDamping"].as<float>();
			props.AngularDamping           = data["AngularDamping"].as<float>();
			props.AllowSleep               = data["AllowSleep"].as<bool>();
			props.Dynamics.IsAwake         = data["Awake"].as<bool>();
			props.FixedRotation            = data["FixedRotation"].as<bool>();
			props.GravityScale             = data["GravityScale"].as<float>();
			props.Bullet                   = data["Bullet"].as<bool>();
			rbc.RigidBody                  = RigidBody2D::Create(props);

			return true;
		}
	};

	template<>
	class ObjectSerializer<Colliders2DComponent>
	{
	public:
		static bool Serialize(YAML::Emitter& out, const Colliders2DComponent* object)
		{
			const Colliders2DComponent& c2c = *object;

			out << YAML::Key << "Enabled" << YAML::Value << c2c.Enabled;

			out << YAML::Key << "Colliders" << YAML::Value << YAML::BeginSeq; // Colliders
			for (const auto& collider : c2c.Colliders)
			{
				const auto& props = collider->GetProps();

				out << YAML::BeginMap;

				out << YAML::Key << "Offset" << YAML::Value << props.Offset;

				out << YAML::Key << "Shape" << YAML::Value << YAML::BeginMap; // Shape
				{
					out << YAML::Key << "Type" << YAML::Value << (int)props.Shape->GetType();
					if (auto boxShape = std::dynamic_pointer_cast<BoxCollisionShape2D>(props.Shape))
					{
						out << YAML::Key << "Size" << YAML::Value << boxShape->GetSize();
						out << YAML::Key << "Rotation" << YAML::Value << boxShape->GetRotation();
					}
					else if (auto circleShape = std::dynamic_pointer_cast<CircleCollisionShape2D>(props.Shape))
					{
						out << YAML::Key << "Radius" << YAML::Value << circleShape->GetRadius();
					}
				}
				out << YAML::EndMap; // Shape

				out << YAML::Key << "IsTrigger" << YAML::Value << props.IsTrigger;
				out << YAML::Key << "Friction" << YAML::Value << props.Friction;
				out << YAML::Key << "Density" << YAML::Value << props.Density;
				out << YAML::Key << "Bounciness" << YAML::Value << props.Bounciness;
				out << YAML::Key << "BouncinessThreshold" << YAML::Value << props.BouncinessThreshold;

				out << YAML::EndMap;
			}
			out << YAML::EndSeq; // Colliders

			return true;
		};

		static bool Deserialize(YAML::Node data, Colliders2DComponent* object)
		{
			Colliders2DComponent& c2c = *object;

			for (auto collider : data["Colliders"])
			{
				Collider2DProps props;

				props.Offset = collider["Offset"].as<Vector2>();

				// Shape
				auto shapeType = (CollisionShape2DType)collider["Shape"]["Type"].as<int>();

				Ref<CollisionShape2D> shape;
				switch (shapeType)
				{
				case CollisionShape2DType::Box:
					shape = BoxCollisionShape2D::Create(collider["Shape"]["Size"].as<Vector2>(), collider["Shape"]["Rotation"].as<float>());
					break;

				case CollisionShape2DType::Circle:
					shape = CircleCollisionShape2D::Create(collider["Shape"]["Radius"].as<float>());
					break;

				default:
					break;
				}

				props.Shape = shape;

				props.Friction = collider["Friction"].as<float>();
				props.Bounciness = collider["Bounciness"].as<float>();
				props.BouncinessThreshold = collider["BouncinessThreshold"].as<float>();

				props.Density = collider["Density"].as<float>();
				props.IsTrigger = collider["IsTrigger"].as<bool>();

				c2c.Colliders.push_back(Collider2D::Create(props));
			}

			return true;
		}
	};
}
