#include "pcheader.h"
#include "SceneSerializer.h"
#include "OverEngine/Core/Serialization/Serializer.h"

#include "Entity.h"
#include "Components.h"
#include "TransformComponent.h"

#include "OverEngine/Assets/Texture2DAsset.h"

#include <fstream>

#include <OverEngine/Core/Serialization/YamlConverters.h>
#include <yaml-cpp/yaml.h>

namespace OverEngine
{
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << YAML::Hex << entity.GetComponent<IDComponent>().ID;

		// TODO: Order matters

		if (entity.HasComponent<NameComponent>())
		{
			out << YAML::Key << "NameComponent";
			out << YAML::BeginMap; // NameComponent

			auto& tag = entity.GetComponent<NameComponent>().Name;
			out << YAML::Key << "Name" << YAML::Value << tag;

			out << YAML::EndMap; // NameComponent
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent" << YAML::BeginMap; // TransformComponent

			auto& tc = entity.GetComponent<TransformComponent>();

			if (auto parent = tc.GetParent())
				out << YAML::Key << "Parent" << YAML::Value << YAML::Hex << parent.GetComponent<IDComponent>().ID;
			else
				out << YAML::Key << "Parent" << YAML::Value << YAML::Hex << YAML::Null;

			out << YAML::Key << "SiblingIndex" << YAML::Value << tc.GetSiblingIndex();

			out << YAML::Key << "Position" << YAML::Value << tc.GetLocalPosition();
			out << YAML::Key << "Rotation" << YAML::Value << tc.GetLocalEulerAngles();
			out << YAML::Key << "Scale" << YAML::Value << tc.GetLocalScale();

			out << YAML::EndMap; // TransformComponent
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto& cc = entity.GetComponent<CameraComponent>();
			out << YAML::Key << "Enabled" << YAML::Value << cc.Enabled;

			out << YAML::Key << "Camera" << YAML::Value << YAML::BeginMap; // Camera
			Serializer::SerializeToYaml(*SceneCamera::Reflect(), &cc.Camera, out);
			out << YAML::EndMap; // Camera

			//out << YAML::Key << "Primary" << YAML::Value << cc.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cc.FixedAspectRatio; // TODO: Fix

			out << YAML::EndMap; // CameraComponent
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent" << YAML::BeginMap; // SpriteRendererComponent

			auto& sp = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Enabled" << YAML::Value << sp.Enabled;

			out << YAML::Key << "Sprite" << YAML::Value;
			if (sp.Sprite)
			{
				auto* asset = sp.Sprite->GetAsset();

				out << YAML::Flow << YAML::BeginMap;
				out << YAML::Key << "Asset" << YAML::Value << YAML::Hex << asset->GetGuid();
				out << YAML::Key << "Texture2D" << YAML::Value << YAML::Hex << asset->GetTextureGuid(sp.Sprite);
				out << YAML::EndMap;
			}
			else
			{
				out << YAML::Null;
			}

			Serializer::SerializeToYaml(*SpriteRendererComponent::Reflect(), &sp, out);

			out << YAML::EndMap; // SpriteRendererComponent
		}

		if (entity.HasComponent<RigidBody2DComponent>())
		{
			out << YAML::Key << "RigidBody2DComponent" << YAML::BeginMap; // RigidBody2DComponent

			auto& rbc = entity.GetComponent<RigidBody2DComponent>();
			out << YAML::Key << "Enabled" << YAML::Value << rbc.Enabled;

			Serializer::SerializeToYaml(*RigidBody2DComponent::Reflect(), &rbc, out);

			out << YAML::EndMap; // RigidBody2DComponent
		}

		if (entity.HasComponent<Colliders2DComponent>())
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
					if (!Serializer::GlobalEnumExists("Collider2DType"))
					{
						Serializer::DefineGlobalEnum("Collider2DType", {
							{ 0, "Box" },
							{ 1, "Circle" }
						});
					}

					out << YAML::Key << "Type" << YAML::Value << Serializer::GetGlobalEnum("Collider2DType")[(int)init.Shape.Type];
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

					if (!Serializer::GlobalEnumExists("SceneCamera::ProjectionType"))
					{
						Serializer::DefineGlobalEnum("SceneCamera::ProjectionType", {
							{ 0, "Orthographic" },
							{ 1, "Perspective" }
						});
					}

					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();

					auto cameraProps = cameraComponent["Camera"];
					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)Serializer::GetGlobalEnumValue("SceneCamera::ProjectionType", cameraProps["ProjectionType"].as<String>()));

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

					if (!Serializer::GlobalEnumExists("TextureWrapping"))
					{
						Serializer::DefineGlobalEnum("TextureWrapping", {
							{ 0, "None" },
							{ 1, "Repeat" },
							{ 2, "MirroredRepeat" },
							{ 3, "ClampToEdge" },
							{ 4, "ClampToBorder" }
						});
					}

					if (!Serializer::GlobalEnumExists("TextureFiltering"))
					{
						Serializer::DefineGlobalEnum("TextureFiltering", {
							{ 0, "None" },
							{ 1, "Nearest" },
							{ 2, "Linear" }
						});
					}

					if (!spriteRendererComponent["Sprite"].IsNull())
					{
						auto sprite = spriteRendererComponent["Sprite"];
						sp.Sprite = Texture2D::CreatePlaceholder(sprite["Asset"].as<uint64_t>(), sprite["Texture2D"].as<uint64_t>());
					}

					sp.Tint = spriteRendererComponent["Tint"].as<Color>();
					sp.Tiling = spriteRendererComponent["Tiling"].as<Vector2>();
					sp.Flip.x = spriteRendererComponent["Flip.x"].as<bool>();
					sp.Flip.y = spriteRendererComponent["Flip.y"].as<bool>();
					sp.Wrapping.x = (TextureWrapping)Serializer::GetGlobalEnumValue("TextureWrapping", spriteRendererComponent["Wrapping.x"].as<String>());
					sp.Wrapping.y = (TextureWrapping)Serializer::GetGlobalEnumValue("TextureWrapping", spriteRendererComponent["Wrapping.y"].as<String>());
					sp.Filtering = (TextureFiltering)Serializer::GetGlobalEnumValue("TextureFiltering", spriteRendererComponent["Filtering"].as<String>());
					sp.AlphaClipThreshold = spriteRendererComponent["AlphaClipThreshold"].as<float>();
					sp.TextureBorderColor.first = spriteRendererComponent["IsOverridingTextureBorderColor"].as<bool>();
					sp.TextureBorderColor.second = spriteRendererComponent["TextureBorderColor"].as<Color>();
				}

				if (auto rigidBody2DComponent = entity["RigidBody2DComponent"])
				{
					auto& rbc = deserializedEntity.AddComponent<RigidBody2DComponent>();

					if (!Serializer::GlobalEnumExists("RigidBody2DType"))
					{
						Serializer::DefineGlobalEnum("RigidBody2DType", {
							{ 0, "Static" },
							{ 1, "Kinematic" },
							{ 2, "Dynamic" }
						});
					}

					rbc.Initializer.Type = (RigidBody2DType)Serializer::GetGlobalEnumValue("RigidBody2DType", rigidBody2DComponent["Type"].as<String>());
					rbc.Initializer.LinearVelocity = rigidBody2DComponent["LinearVelocity"].as<Vector2>();
					rbc.Initializer.AngularVelocity = rigidBody2DComponent["AngularVelocity"].as<float>();
					rbc.Initializer.LinearDamping = rigidBody2DComponent["LinearDamping"].as<float>();
					rbc.Initializer.AngularDamping = rigidBody2DComponent["AngularDamping"].as<float>();
					rbc.Initializer.AllowSleep = rigidBody2DComponent["AllowSleep"].as<bool>();
					rbc.Initializer.Awake = rigidBody2DComponent["Awake"].as<bool>();
					rbc.Initializer.FixedRotation = rigidBody2DComponent["FixedRotation"].as<bool>();
					rbc.Initializer.GravityScale = rigidBody2DComponent["GravityScale"].as<float>();
					rbc.Initializer.Bullet = rigidBody2DComponent["Bullet"].as<bool>();
				}

				if (auto colliders2DComponent = entity["Colliders2DComponent"])
				{
					auto& c2c = deserializedEntity.AddComponent<Colliders2DComponent>();

					for (auto collider : colliders2DComponent["Colliders"])
					{
						Collider2DProps props;

						props.Offset = collider["Offset"].as<Vector2>();
						props.Rotation = collider["Rotation"].as<float>();

						if (!Serializer::GlobalEnumExists("Collider2DType"))
						{
							Serializer::DefineGlobalEnum("Collider2DType", {
								{ 0, "Box" },
								{ 1, "Circle" }
							});
						}

						props.Shape.Type = (Collider2DType)Serializer::GetGlobalEnumValue("Collider2DType", collider["Shape"]["Type"].as<String>());
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
