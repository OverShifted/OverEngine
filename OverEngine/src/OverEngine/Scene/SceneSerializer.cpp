#include "pcheader.h"
#include "SceneSerializer.h"

#include "OverEngine/Core/Runtime/Serialization/Serializer.h"
#include "OverEngine/Core/AssetManagement/Asset.h"
#include "OverEngine/Core/AssetManagement/AssetDatabase.h"

#include "Entity.h"
#include "Components.h"
#include "TransformComponent.h"

#include <fstream>

#include "OverEngine/Core/Runtime/Serialization/YamlConverters.h"
#include <yaml-cpp/yaml.h>

namespace OverEngine
{
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap;

		// Primary components:

		// UUID
		out << YAML::Key << "Entity" << YAML::Value << YAML::Hex << entity.GetComponent<IDComponent>().ID;

		// NameComponent
		if (entity.HasComponent<NameComponent>())
		{
			out << YAML::Key << "NameComponent" << YAML::BeginMap; // NameComponent

			out << YAML::Key << "Name" << YAML::Value << entity.GetComponent<NameComponent>().Name;

			out << YAML::EndMap; // NameComponent
		}

		// TransformComponent
		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent" << YAML::BeginMap; // TransformComponent

			auto& tc = entity.GetComponent<TransformComponent>();

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

			out << YAML::EndMap; // TransformComponent
		}

		// Other Components
		for (auto typeID : entity.GetComponentsTypeIDList())
		{
			if (typeID == GetComponentTypeID<CameraComponent>())
			{
				out << YAML::Key << "CameraComponent";
				out << YAML::BeginMap; // CameraComponent

				auto& cc = entity.GetComponent<CameraComponent>();
				out << YAML::Key << "Enabled" << YAML::Value << cc.Enabled;

				out << YAML::Key << "Camera" << YAML::Value << YAML::BeginMap; // Camera
				Serializer::SerializeToYaml(&cc.Camera, out, false);
				out << YAML::EndMap; // Camera

				out << YAML::Key << "FixedAspectRatio" << YAML::Value << cc.FixedAspectRatio;

				out << YAML::EndMap; // CameraComponent
			}

			else if (typeID == GetComponentTypeID<SpriteRendererComponent>())
			{
				out << YAML::Key << "SpriteRendererComponent" << YAML::BeginMap; // SpriteRendererComponent

				auto& sp = entity.GetComponent<SpriteRendererComponent>();
				out << YAML::Key << "Enabled" << YAML::Value << sp.Enabled;

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

				Serializer::SerializeToYaml(&sp, out, false);

				out << YAML::EndMap; // SpriteRendererComponent
			}

			else if (typeID == GetComponentTypeID<RigidBody2DComponent>())
			{
				out << YAML::Key << "RigidBody2DComponent" << YAML::BeginMap; // RigidBody2DComponent

				auto& rbc = entity.GetComponent<RigidBody2DComponent>();
				out << YAML::Key << "Enabled" << YAML::Value << rbc.Enabled;

				Serializer::SerializeToYaml(&rbc.Initializer, out, false);

				out << YAML::EndMap; // RigidBody2DComponent
			}

			else if (typeID == GetComponentTypeID<Colliders2DComponent>())
			{
				out << YAML::Key << "Colliders2DComponent" << YAML::BeginMap; // Colliders2DComponent

				auto& c2c = entity.GetComponent<Colliders2DComponent>();
				out << YAML::Key << "Enabled" << YAML::Value << c2c.Enabled;

				out << YAML::Key << "Colliders" << YAML::Value << YAML::BeginSeq;
				for (const auto& collider : c2c.Colliders)
				{
					const auto& init = collider.Initializer;

					out << YAML::BeginMap;

					out << YAML::Key << "Offset" << YAML::Value << init.Offset;
					out << YAML::Key << "Rotation" << YAML::Value << init.Rotation;

					out << YAML::Key << "Shape" << YAML::Value << YAML::BeginMap;
					{
						out << YAML::Key << "Type" << YAML::Value << Reflection::EnumClassResolver::GetNameByValue(init.Shape.Type);
						out << YAML::Key << "BoxSize" << YAML::Value << init.Shape.BoxSize;
						out << YAML::Key << "CircleRadius" << YAML::Value << init.Shape.CircleRadius;
					}
					out << YAML::EndMap;

					out << YAML::Key << "IsTrigger" << YAML::Value << init.IsTrigger;

					out << YAML::Key << "Friction" << YAML::Value << init.Friction;
					out << YAML::Key << "Density" << YAML::Value << init.Density;

					out << YAML::Key << "Bounciness" << YAML::Value << init.Bounciness;
					out << YAML::Key << "BouncinessThreshold" << YAML::Value << init.BouncinessThreshold;

					out << YAML::EndMap;
				}
				out << YAML::EndSeq;

				out << YAML::EndMap; // Colliders2DComponent
			}
		}

		out << YAML::EndMap; // Entity
	}

	void SceneSerializer::Serialize(const String& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		Vector<entt::entity> entities;
		m_Scene->m_Registry.each([&](auto entityID)
		{
			entities.insert(entities.begin(), entityID);
		});

		for (const auto& entity : entities)
		{
			SerializeEntity(out, { entity, m_Scene.get() });
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	bool SceneSerializer::Deserialize(const String& filepath)
	{
		YAML::Node data = YAML::LoadFile(filepath);

		if (!data["Scene"])
			return false;
		
		String sceneName = data["Scene"].as<String>();

		auto entities = data["Entities"];

		UnorderedMap<uint64_t, entt::entity> uuids;
		uuids.reserve(entities.size());

		UnorderedMap<entt::entity, uint32_t> siblingIndices;
		siblingIndices.reserve(entities.size());

		UnorderedMap<entt::entity, uint64_t> parents;
		parents.reserve(entities.size());

		if (entities)
		{
			for (auto entity : entities)
			{
				String name;
				if (auto nameComponent = entity["NameComponent"])
					name = nameComponent["Name"].as<String>();

				uint64_t uuid = entity["Entity"].as<uint64_t>();
				Entity deserializedEntity = m_Scene->CreateEntity(name, uuid);
				uuids[uuid] = deserializedEntity.GetRuntimeID();

				if (auto transformComponent = entity["TransformComponent"])
				{
					// Entities always have transforms
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();

					if (!transformComponent["Parent"].IsNull())
						parents[deserializedEntity.GetRuntimeID()] = transformComponent["Parent"].as<uint64_t>();

					siblingIndices[deserializedEntity.GetRuntimeID()] = transformComponent["SiblingIndex"].as<uint32_t>();

					tc.SetLocalPosition(transformComponent["Position"].as<Vector3>());
					tc.SetLocalEulerAngles(transformComponent["Rotation"].as<Vector3>());
					tc.SetLocalScale(transformComponent["Scale"].as<Vector3>());
				}

				if (auto cameraComponent = entity["CameraComponent"])
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();

					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();

					auto cameraProps = cameraComponent["Camera"];
					cc.Camera.SetProjectionType(Reflection::EnumClassResolver::GetValueByName<SceneCamera::ProjectionType>(cameraProps["ProjectionType"].as<String>()).value());

					cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

					cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					cc.Camera.SetClearFlags(cameraProps["ClearFlags"].as<uint8_t>());
					cc.Camera.SetClearColor(cameraProps["ClearColor"].as<Color>());
				}

				if (auto spriteRendererComponent = entity["SpriteRendererComponent"])
				{
					auto& sp = deserializedEntity.AddComponent<SpriteRendererComponent>();

					if (!spriteRendererComponent["Sprite"].IsNull())
					{
						uint32_t refID = spriteRendererComponent["Sprite"]["Reference"].as<uint32_t>();
						auto asset = data["References"][refID];

						String typeName = asset["Type"].as<String>();
						if (typeName == Texture2D::GetStaticClassName())
							sp.Sprite = AssetDatabase::RegisterAndGet<Texture2D>(asset["Guid"].as<uint64_t>());
					}

					sp.Tint = spriteRendererComponent["Tint"].as<Color>();
					sp.Tiling = spriteRendererComponent["Tiling"].as<Vector2>();

					if (spriteRendererComponent["Flip.x"].as<bool>())
						sp.Flip |= TextureFlip_X;
					if (spriteRendererComponent["Flip.y"].as<bool>())
						sp.Flip |= TextureFlip_Y;
				}

				if (auto rigidBody2DComponent = entity["RigidBody2DComponent"])
				{
					auto& rbc = deserializedEntity.AddComponent<RigidBody2DComponent>();

					rbc.Initializer.Type = Reflection::EnumClassResolver::GetValueByName<RigidBody2DType>(rigidBody2DComponent["Type"].as<String>()).value();
					rbc.Initializer.LinearVelocity  = rigidBody2DComponent["LinearVelocity"].as<Vector2>();
					rbc.Initializer.AngularVelocity = rigidBody2DComponent["AngularVelocity"].as<float>();
					rbc.Initializer.LinearDamping   = rigidBody2DComponent["LinearDamping"].as<float>();
					rbc.Initializer.AngularDamping  = rigidBody2DComponent["AngularDamping"].as<float>();
					rbc.Initializer.AllowSleep      = rigidBody2DComponent["AllowSleep"].as<bool>();
					rbc.Initializer.Awake           = rigidBody2DComponent["Awake"].as<bool>();
					rbc.Initializer.FixedRotation   = rigidBody2DComponent["FixedRotation"].as<bool>();
					rbc.Initializer.GravityScale    = rigidBody2DComponent["GravityScale"].as<float>();
					rbc.Initializer.Bullet          = rigidBody2DComponent["Bullet"].as<bool>();
				}

				if (auto colliders2DComponent = entity["Colliders2DComponent"])
				{
					auto& c2c = deserializedEntity.AddComponent<Colliders2DComponent>();

					for (auto collider : colliders2DComponent["Colliders"])
					{
						Collider2DProps props;

						props.Offset = collider["Offset"].as<Vector2>();
						props.Rotation = collider["Rotation"].as<float>();

						props.Shape.Type = Reflection::EnumClassResolver::GetValueByName<Collider2DType>(collider["Shape"]["Type"].as<String>()).value();
						props.Shape.BoxSize = collider["Shape"]["BoxSize"].as<Vector2>();
						props.Shape.CircleRadius = collider["Shape"]["CircleRadius"].as<float>();

						props.IsTrigger = collider["IsTrigger"].as<bool>();

						props.Friction = collider["Friction"].as<float>();
						props.Density = collider["Density"].as<float>();
						props.Bounciness = collider["Bounciness"].as<float>();
						props.BouncinessThreshold = collider["BouncinessThreshold"].as<float>();

						c2c.Colliders.push_back({ props, nullptr });
					}
				}
			}
		}

		// Attach to parents
		for (const auto& p : parents)
			m_Scene->m_Registry.get<TransformComponent>(p.first).SetParent({ uuids[p.second], m_Scene.get() });

		// Set sibling indices
		m_Scene->m_Registry.view<TransformComponent>().each([&siblingIndices](entt::entity entity, auto& tc)
		{
			tc.SetSiblingIndex(siblingIndices[entity]);
		});

		return true;
	}
}
