#include "pcheader.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "TransformComponent.h"

#include "OverEngine/Renderer/Renderer2D.h"
#include "OverEngine/Physics/PhysicsWorld2D.h"

#include <fstream>

#include <json.hpp>

#include "OverEngine/Core/Serialization/YamlConverters.h"
#include <yaml-cpp/yaml.h>

namespace OverEngine
{
	Scene::Scene(const SceneSettings& settings)
		: m_PhysicsWorld2D(settings.physics2DSettings.gravity) // TODO: Move to OnScenePlay()
	{
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const String& name /*= String()*/)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<BaseComponent>(name.empty() ? "Entity" : name);
		entity.AddComponent<TransformComponent>();
		m_RootHandles.push_back(entity.GetRuntimeID());
		return entity;
	}

	Entity Scene::CreateEntity(Entity& parent, const String& name /*= String()*/)
	{
		OE_CORE_ASSERT(parent, "Parent is null!");
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<BaseComponent>(name.empty() ? "Entity" : name);
		entity.AddComponent<TransformComponent>(parent);
		return entity;
	}

	void Scene::OnUpdate(TimeStep deltaTime, Vector2 renderSurface)
	{
		OnPhysicsUpdate(deltaTime); // TODO: use a FixedUpdate (just like Unity)
		OnRender(renderSurface);
	}

	void Scene::OnPhysicsUpdate(TimeStep DeltaTime)
	{
		/////////////////////////////////////////////////////
		// Physics & Transform //////////////////////////////
		/////////////////////////////////////////////////////

		// TODO: Mix everything in a Physics(2D)Component
		// and remove this code
		{
			auto view = m_Registry.view<PhysicsColliders2DComponent>();
			for (auto entity : view)
			{
				auto colliders = view.get<PhysicsColliders2DComponent>(entity);
				for (const auto& collider : colliders.Colliders)
				{
					if (collider->m_Changed)
					{
						if (colliders.AttachedBody->m_BodyHandle)
						{
							colliders.AttachedBody->RemoveCollider(collider);
							colliders.AttachedBody->AddCollider(collider);
						}
					}
				}
			}
		}

		// Update Physics
		m_PhysicsWorld2D.OnUpdate(DeltaTime, 8, 3);

		{
			auto group = m_Registry.group<PhysicsBody2DComponent>(entt::get<TransformComponent>);
			for (auto entity : group)
			{
				auto& body2D = group.get<PhysicsBody2DComponent>(entity);
				auto& transform = group.get<TransformComponent>(entity);

				if (body2D.Enabled)
				{
					if (transform.m_ChangedFlags & TransformComponent::ChangedFlags_ChangedForPhysics)
					{
						// Push changes to Box2D world
						const auto& pos = transform.GetPosition();
						body2D.Body->SetPosition({ pos.x, pos.y });
						body2D.Body->SetRotation(glm::radians(transform.GetEulerAngles().z));
					}
					else
					{
						// Push changes to OverEngine transform system
						transform.SetPosition(Vector3(body2D.Body->GetPosition(), 0));
						const auto& angles = transform.GetEulerAngles();
						transform.SetEulerAngles({ angles.x, angles.y, glm::degrees(body2D.Body->GetRotation()) });
					}

					// In both cases; we need to perform this
					// 1. we've pushed the changes to physics system and we need to remove the flag
					// 2. we've pushed the changes to OverEngine's transform system and it added the
					//    flag which we don't want
					transform.m_ChangedFlags ^= TransformComponent::ChangedFlags_ChangedForPhysics;
				}
			}
		}
	}

	bool Scene::OnRender(Vector2 renderSurface)
	{
		/////////////////////////////////////////////////////
		// Render ///////////////////////////////////////////
		/////////////////////////////////////////////////////

		auto group = m_Registry.group<TransformComponent>(entt::get<CameraComponent>);
		uint32_t activeCameras = 0;
		for (auto entity : group)
		{
			auto& transform = group.get<TransformComponent>(entity);
			auto& camera = group.get<CameraComponent>(entity);

			if (!camera.Enabled)
				continue;

			activeCameras++;

			camera.Camera.SetViewportSize((uint32_t)renderSurface.x, (uint32_t)renderSurface.y);

			RenderCommand::SetClearColor(camera.Camera.GetClearColor());
			RenderCommand::Clear(camera.Camera.GetClearFlags());
				
			Renderer2D::BeginScene(glm::inverse(transform.GetLocalToWorld()), camera.Camera);

			{
				auto spritesGroup = m_Registry.group<SpriteRendererComponent>(entt::get<TransformComponent>);
				for (auto sp : spritesGroup)
				{
					auto& sprite = spritesGroup.get<SpriteRendererComponent>(sp);
					if (sprite.Enabled)
					{
						auto& sptransform = spritesGroup.get<TransformComponent>(sp);

						if (sprite.Sprite)
						{
							TexturedQuadExtraData data;
							data.tint = sprite.Tint;
							data.tilingFactorX = sprite.TilingFactorX;
							data.tilingFactorY = sprite.TilingFactorY;
							data.flipX = sprite.FlipX;
							data.flipY = sprite.FlipY;
							data.overrideSTextureWrapping = sprite.OverrideSWrapping;
							data.overrideTTextureWrapping = sprite.OverrideTWrapping;
							data.overrideTextureFiltering = sprite.OverrideFiltering;
							data.alphaClippingThreshold = sprite.AlphaClippingThreshold;
							if (sprite.IsOverridingTextureBorderColor)
								data.overrideTextureBorderColor = sprite.OverrideTextureBorderColor;

							Renderer2D::DrawQuad(sptransform, sprite.Sprite->GetAsset(), data);
						}
						else
						{
							Renderer2D::DrawQuad(sptransform, sprite.Tint, sprite.AlphaClippingThreshold);
						}
					}
				}
			}

			Renderer2D::EndScene();
		}

		// Returns false if nothing is rendered
		return activeCameras;
	}

	void Scene::OnRender(const SceneCamera& camera, const Mat4x4& cameraTransform)
	{
		RenderCommand::SetClearColor(camera.GetClearColor());
		RenderCommand::Clear(camera.GetClearFlags());

		Renderer2D::BeginScene(glm::inverse(cameraTransform), camera);

		auto spritesGroup = m_Registry.group<SpriteRendererComponent>(entt::get<TransformComponent>);
		for (auto sp : spritesGroup)
		{
			auto& sprite = spritesGroup.get<SpriteRendererComponent>(sp);
			if (sprite.Enabled)
			{
				auto& sptransform = spritesGroup.get<TransformComponent>(sp);

				if (sprite.Sprite)
				{
					TexturedQuadExtraData data;
					data.tint = sprite.Tint;
					data.tilingFactorX = sprite.TilingFactorX;
					data.tilingFactorY = sprite.TilingFactorY;
					data.flipX = sprite.FlipX;
					data.flipY = sprite.FlipY;
					data.overrideSTextureWrapping = sprite.OverrideSWrapping;
					data.overrideTTextureWrapping = sprite.OverrideTWrapping;
					data.overrideTextureFiltering = sprite.OverrideFiltering;
					data.alphaClippingThreshold = sprite.AlphaClippingThreshold;
					if (sprite.IsOverridingTextureBorderColor)
						data.overrideTextureBorderColor = sprite.OverrideTextureBorderColor;

					Renderer2D::DrawQuad(sptransform, sprite.Sprite->GetAsset(), data);
				}
				else
				{
					Renderer2D::DrawQuad(sptransform, sprite.Tint, sprite.AlphaClippingThreshold);
				}
			}
		}

		Renderer2D::EndScene();
	}

	void Scene::Dump(const String& filePath)
	{
		YAML::Node sceneNode;

		// Physics2D Settings
		sceneNode["Physics2DSettings"]["Gravity"] = GetPhysicsWorld2D().GetGravity();

		Vector<entt::entity> entityIDs;
		Vector<YAML::Node> entityNodes;
		entityIDs.reserve(GetEntityCount());
		entityNodes.reserve(GetEntityCount());

		Each([&](Entity entity)
			{
				entityIDs.push_back(entity.GetRuntimeID());
			});

		Each([&](Entity entity)
		{
			const auto& componentList = entity.GetComponentsTypeIDList();

			entityNodes.push_back(YAML::Node());
			auto& entityNode = entityNodes[entityNodes.size() - 1];

			const auto& transform = entity.GetComponent<TransformComponent>();

			// Name and Guid
			const auto& base = entity.GetComponent<BaseComponent>();
			entityNode["Name"] = base.Name;
			entityNode["GUID"] = base.ID.ToString();

			// Family
			if (!transform.GetParent())
			{
				entityNode["Parent"] = YAML::Node(YAML::NodeType::Null);
			}
			else
			{
				auto it = std::find(entityIDs.begin(), entityIDs.end(), transform.GetParent().GetRuntimeID());
				OE_CORE_ASSERT(it != entityIDs.end(), "Parent not found!");
				entityNode["Parent"] = it - entityIDs.begin();
			}

			auto transformAlias = entityNode["Transform"];
			Serializer::SerializeToYaml(*TransformComponent::Reflect(), (void*)&transform, transformAlias);

			#define SERIALIZE_COMPONENT(type) (componentTypeID == GetComponentTypeID<type>()) {\
			componentNode["TypeName"] = type::GetStaticName();\
			Serializer::SerializeToYaml(*type::Reflect(), (void*)&entity.GetComponent<type>(), componentNode); }

			entityNode["Components"] = Vector<YAML::Node>();
			for (const auto& componentTypeID : componentList)
			{
				YAML::Node componentNode(YAML::NodeType::Null);

				if SERIALIZE_COMPONENT(CameraComponent)
				else if SERIALIZE_COMPONENT(SpriteRendererComponent)

					if (!componentNode.IsNull())
						entityNode["Components"].push_back(componentNode);
			}
		});

		sceneNode["Entities"] = entityNodes;

		std::ofstream sceneFile(filePath);
		sceneFile << sceneNode;
		sceneFile.flush();
		sceneFile.close();
	}

	uint32_t Scene::GetEntityCount() const
	{
		return (uint32_t)m_Registry.size<BaseComponent>();
	}

	////////////////////////////////////////////////////////////
	// Scene loading and saving ////////////////////////////////
	////////////////////////////////////////////////////////////

	Ref<Scene> CreateSceneOnDisk(const String& path)
	{
		OE_DEBUGBREAK;
		// Invalid Code
		OE_CORE_ASSERT(false, "Deprecated");
		OE_CORE_ERROR("Deprecated");
		nlohmann::json json;

		if (path.size() == 0)
			return nullptr;

		json["Settings"]["Physics2DSettings"]["Gravity"]["x"] = 0.0f;
		json["Settings"]["Physics2DSettings"]["Gravity"]["y"] = -9.8f;

		json["Entities"] = nlohmann::json::array();

		std::ofstream sceneFile(path);

		sceneFile << json.dump(1, '\t');
		sceneFile.flush();
		sceneFile.close();

		return LoadSceneFromFile(path);
	}

	Ref<Scene> LoadSceneFromFile(const String& path)
	{
		YAML::Node sceneNode = YAML::LoadFile(path);

		SceneSettings settings;
		settings.physics2DSettings.gravity = sceneNode["Physics2DSettings"]["Gravity"].as<Vector2>();
		Ref<Scene> scene = CreateRef<Scene>(settings);

		Vector<Entity> entities;
		Map<uint32_t, uint32_t> entityParentAssignList;

		for (uint32_t i = 0; i < sceneNode["Entities"].size(); i++)
		{
			Entity entity = scene->CreateEntity(sceneNode["Entities"][i]["Name"].as<String>());
			entities.push_back(entity);

			auto it = entityParentAssignList.begin();
			while (it != entityParentAssignList.end())
			{
				if (it->second <= i)
				{
					entities[it->first].GetComponent<TransformComponent>().SetParent(entities[it->second]);
					it = entityParentAssignList.erase(it);
				}
				else
				{
					it++;
				}
			}

			// Family
			if (!sceneNode["Entities"][i]["Parent"].IsNull())
			{
				uint32_t parentNodeInt = sceneNode["Entities"][i]["Parent"].as<uint32_t>();
				if (parentNodeInt < i)
					entity.GetComponent<TransformComponent>().SetParent(entities[parentNodeInt]);
				else
					entityParentAssignList[i] = parentNodeInt;
			}

			auto& base = entity.GetComponent<BaseComponent>();
			base.ID = Guid(sceneNode["Entities"][i]["GUID"].as<String>());

			// Transform
			auto& transform = entity.GetComponent<TransformComponent>();

			transform.SetLocalPosition(sceneNode["Entities"][i]["Transform"]["m_LocalPosition"].as<Vector3>());
			transform.SetLocalEulerAngles(sceneNode["Entities"][i]["Transform"]["m_LocalEulerAngles"].as<Vector3>());
			transform.SetLocalScale(sceneNode["Entities"][i]["Transform"]["m_LocalScale"].as<Vector3>());

			for (uint32_t j = 0; j < sceneNode["Entities"][i]["Components"].size(); j++)
			{
				if (sceneNode["Entities"][i]["Components"][j]["TypeName"].as<String>() == "CameraComponent")
				{
					auto& camera = entity.AddComponent<CameraComponent>();

					if (!Serializer::GlobalEnumExists("ProjectionType"))
					{
						Serializer::DefineGlobalEnum("ProjectionType", {
							{ 0, "Orthographic" },
							{ 1, "Perspective" }
						});
					}

					camera.Camera.SetProjectionType((SceneCamera::ProjectionType)Serializer::GetGlobalEnumValue("ProjectionType", sceneNode["Entities"][i]["Components"][j]["m_ProjectionType"].as<String>()));

					camera.Camera.SetPerspectiveVerticalFOV(sceneNode["Entities"][i]["Components"][j]["m_PerspectiveFOV"].as<float>());
					camera.Camera.SetPerspectiveNearClip(sceneNode["Entities"][i]["Components"][j]["m_PerspectiveNear"].as<float>());
					camera.Camera.SetPerspectiveFarClip(sceneNode["Entities"][i]["Components"][j]["m_PerspectiveFar"].as<float>());

					camera.Camera.SetOrthographicSize(sceneNode["Entities"][i]["Components"][j]["m_OrthographicSize"].as<float>());
					camera.Camera.SetOrthographicNearClip(sceneNode["Entities"][i]["Components"][j]["m_OrthographicNear"].as<float>());
					camera.Camera.SetOrthographicFarClip(sceneNode["Entities"][i]["Components"][j]["m_OrthographicFar"].as<float>());

					camera.Camera.SetClearFlags(sceneNode["Entities"][i]["Components"][j]["m_ClearFlags"].as<uint8_t>());
					camera.Camera.SetClearColor(sceneNode["Entities"][i]["Components"][j]["m_ClearColor"].as<Color>());
				}
				else if (sceneNode["Entities"][i]["Components"][j]["TypeName"].as<String>() == "SpriteRendererComponent")
				{
					auto& sp = entity.AddComponent<SpriteRendererComponent>();

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

					sp.Tint = sceneNode["Entities"][i]["Components"][j]["Tint"].as<Color>();
					sp.TilingFactorX = sceneNode["Entities"][i]["Components"][j]["TilingFactorX"].as<float>();
					sp.TilingFactorY = sceneNode["Entities"][i]["Components"][j]["TilingFactorY"].as<float>();
					sp.FlipX = sceneNode["Entities"][i]["Components"][j]["FlipX"].as<bool>();
					sp.FlipY = sceneNode["Entities"][i]["Components"][j]["FlipY"].as<bool>();
					sp.OverrideSWrapping = (TextureWrapping)Serializer::GetGlobalEnumValue("TextureWrapping", sceneNode["Entities"][i]["Components"][j]["OverrideSWrapping"].as<String>());
					sp.OverrideTWrapping = (TextureWrapping)Serializer::GetGlobalEnumValue("TextureWrapping", sceneNode["Entities"][i]["Components"][j]["OverrideTWrapping"].as<String>());
					sp.OverrideFiltering = (TextureFiltering)Serializer::GetGlobalEnumValue("TextureFiltering", sceneNode["Entities"][i]["Components"][j]["OverrideFiltering"].as<String>());
					sp.AlphaClippingThreshold = sceneNode["Entities"][i]["Components"][j]["AlphaClippingThreshold"].as<float>();
					sp.IsOverridingTextureBorderColor = sceneNode["Entities"][i]["Components"][j]["IsOverridingTextureBorderColor"].as<bool>();
					sp.OverrideTextureBorderColor = sceneNode["Entities"][i]["Components"][j]["OverrideTextureBorderColor"].as<Color>();
				}
			}
		}

		return scene;
	}
}
