#include "pcheader.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"
#include "TransformComponent.h"
#include "ComponentSerializer.h"

#include "OverEngine/Core/Runtime/Serialization/ObjectSerializer.h"

#include "OverEngine/Core/Runtime/Serialization/YamlConverters.h"
#include <yaml-cpp/yaml.h>

#include <fstream>

namespace OverEngine
{
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap;

		// Primary components

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
			ObjectSerializer<TransformComponent>::Serialize(out, &tc);

			out << YAML::EndMap; // TransformComponent
		}

		// Other Components
		for (auto typeID : entity.GetComponentsTypeIDList())
		{
			if (typeID == GetComponentTypeID<CameraComponent>())
			{
				out << YAML::Key << "CameraComponent" << YAML::BeginMap; // CameraComponent

				auto& cc = entity.GetComponent<CameraComponent>();
				ObjectSerializer<CameraComponent>::Serialize(out, &cc);

				out << YAML::EndMap; // CameraComponent
			}

			else if (typeID == GetComponentTypeID<SpriteRendererComponent>())
			{
				out << YAML::Key << "SpriteRendererComponent" << YAML::BeginMap; // SpriteRendererComponent

				auto& sp = entity.GetComponent<SpriteRendererComponent>();
				ObjectSerializer<SpriteRendererComponent>::Serialize(out, &sp);

				out << YAML::EndMap; // SpriteRendererComponent
			}

			else if (typeID == GetComponentTypeID<RigidBody2DComponent>())
			{
				out << YAML::Key << "RigidBody2DComponent" << YAML::BeginMap; // RigidBody2DComponent

				auto& rbc = entity.GetComponent<RigidBody2DComponent>();
				ObjectSerializer<RigidBody2DComponent>::Serialize(out, &rbc);

				out << YAML::EndMap; // RigidBody2DComponent
			}

			else if (typeID == GetComponentTypeID<Collider2DComponent>())
			{
				out << YAML::Key << "Collider2DComponent" << YAML::BeginMap; // Collider2DComponent

				auto& c2c = entity.GetComponent<Collider2DComponent>();
				ObjectSerializer<Collider2DComponent>::Serialize(out, &c2c);

				out << YAML::EndMap; // Collider2DComponent
			}
		}

		out << YAML::EndMap; // Entity
	}

	void SceneSerializer::Serialize(const String& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		m_Scene->m_Registry.each([&](auto entityID)
		{
			SerializeEntity(out, { entityID, m_Scene.get() });
		});

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
					ObjectSerializer<CameraComponent>::Deserialize(cameraComponent, &cc);
				}

				if (auto spriteRendererComponent = entity["SpriteRendererComponent"])
				{
					auto& sp = deserializedEntity.AddComponent<SpriteRendererComponent>();
					ObjectSerializer<SpriteRendererComponent>::Deserialize(spriteRendererComponent, &sp);
				}

				if (auto rigidBody2DComponent = entity["RigidBody2DComponent"])
				{
					auto& rbc = deserializedEntity.AddComponent<RigidBody2DComponent>();
					ObjectSerializer<RigidBody2DComponent>::Deserialize(rigidBody2DComponent, &rbc);
				}

				if (auto colliders2DComponent = entity["Collider2DComponent"])
				{
					auto& c2c = deserializedEntity.AddComponent<Collider2DComponent>();
					ObjectSerializer<Collider2DComponent>::Deserialize(colliders2DComponent, &c2c);
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
