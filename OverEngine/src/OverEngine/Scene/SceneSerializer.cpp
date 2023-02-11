#include "pcheader.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"
#include "TransformComponent.h"
#include "ComponentSerializer.h"

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

		out << YAML::Key << "Entity" << YAML::Value << YAML::Hex << entity.GetComponent<IDComponent>().ID;

		// Primary components
		#define SERIALIZE_PRIMARY_COMPONENT(T) \
			if (entity.HasComponent<T>())      \
				out << YAML::Key << #T << entity.GetComponent<T>();

		SERIALIZE_PRIMARY_COMPONENT(NameComponent)
		SERIALIZE_PRIMARY_COMPONENT(TransformComponent)

		// Other Components
		for (auto typeID : entity.GetComponentsTypeIDList())
		{
			#define SERIALIZE_COMPONENT(T)             \
				if (typeID == GetComponentTypeID<T>()) \
					out << YAML::Key << #T << entity.GetComponent<T>();

			SERIALIZE_COMPONENT(CameraComponent)
			SERIALIZE_COMPONENT(SpriteRendererComponent)
			SERIALIZE_COMPONENT(RigidBody2DComponent)
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const String&filepath)
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
				entt::entity runtimeID = deserializedEntity.GetRuntimeID();
				uuids[uuid] = runtimeID;

				if (auto transformComponent = entity["TransformComponent"])
				{
					// Entities always have transforms
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();

					if (!transformComponent["Parent"].IsNull())
						parents[runtimeID] = transformComponent["Parent"].as<uint64_t>();

					siblingIndices[runtimeID] = transformComponent["SiblingIndex"].as<uint32_t>();

					tc.SetLocalPosition(transformComponent["Position"].as<Vector3>());
					tc.SetLocalEulerAngles(transformComponent["Rotation"].as<Vector3>());
					tc.SetLocalScale(transformComponent["Scale"].as<Vector3>());
				}

				#define DESERIALIZE_COMPONENT(T) \
					if (auto comp = entity[#T])  \
						YAML::convert<T>::decode(comp, deserializedEntity.AddComponent<T>());

				DESERIALIZE_COMPONENT(CameraComponent)
				DESERIALIZE_COMPONENT(SpriteRendererComponent)
				DESERIALIZE_COMPONENT(RigidBody2DComponent)
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
