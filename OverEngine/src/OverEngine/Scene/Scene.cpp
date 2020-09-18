#include "pcheader.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"

#include "OverEngine/Renderer/Renderer2D.h"
#include "OverEngine/Physics/PhysicsWorld2D.h"

#include <fstream>
#include <json.hpp>

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
		m_RootEntities.push_back(entity);
		return entity;
	}

	Entity Scene::CreateEntity(Entity& parent, const String& name /*= String()*/)
	{
		OE_CORE_ASSERT(parent, "Parent is null!");
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<BaseComponent>(name.empty() ? "Entity" : name, parent);
		entity.AddComponent<TransformComponent>();
		return entity;
	}

	void Scene::OnUpdate(TimeStep deltaTime, Vector2 renderSurface)
	{
		OnPhysicsUpdate(deltaTime); // TODO: use a FixedUpdate (just like Unity)
		OnTransformUpdate();
		OnRender(renderSurface);
	}

	void Scene::OnPhysicsUpdate(TimeStep DeltaTime)
	{
		/////////////////////////////////////////////////////
		// Physics & Transform //////////////////////////////
		/////////////////////////////////////////////////////

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

		m_PhysicsWorld2D.OnUpdate(DeltaTime, 8, 3);

		{
			auto group = m_Registry.group<PhysicsBody2DComponent>(entt::get<TransformComponent>);
			for (auto entity : group)
			{
				auto& body2D = group.get<PhysicsBody2DComponent>(entity);
				auto& transform = group.get<TransformComponent>(entity);

				if (body2D.Enabled)
				{
					if (!transform.m_ChangedByPhysics && transform.m_Changed)
					{
						body2D.Body->SetPosition({ transform.GetPosition().x, transform.GetPosition().y });
						body2D.Body->SetRotation(QuaternionEulerAnglesRadians(transform.GetRotation()).z);
						transform.m_ChangedByPhysics = false;
					}
				
					transform.SetPosition(Vector3(body2D.Body->GetPosition(), 0));
					transform.SetRotation(QuaternionEuler(Vector3(0.0f, 0.0f, glm::degrees(body2D.Body->GetRotation()))));
					transform.m_ChangedByPhysics = true;
				}
			}
		}
	}

	void Scene::OnTransformUpdate()
	{
		{
			auto view = m_Registry.view<TransformComponent>();
			for (auto entity : view)
			{
				TransformComponent& transform = view.get<TransformComponent>(entity);

				if (!transform.IsChanged())
					continue;

				transform.m_Changed = false;
				transform.RecalculateTransformationMatrix();
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

			camera.Camera.SetAspectRatio(renderSurface.x / renderSurface.y);

			RenderCommand::SetClearColor(camera.Camera.GetClearColor());
			RenderCommand::Clear(camera.Camera.GetIsClearingColor(), camera.Camera.GetIsClearingDepth());
				
			Renderer2D::BeginScene(glm::inverse(transform.GetTransformationMatrix()), camera.Camera);

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
							if (sprite.IsOverrideTextureBorderColor)
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

	uint32_t Scene::GetEntityCount()
	{
		return (uint32_t)m_Registry.size<BaseComponent>();
	}

	////////////////////////////////////////////////////////////
	// Scene loading and saving ////////////////////////////////
	////////////////////////////////////////////////////////////

	Ref<Scene> CreateSceneOnDisk(const String& path)
	{
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
		// read a JSON file
		std::ifstream sceneFile(path);
		nlohmann::json sceneJson;
		sceneFile >> sceneJson;

		auto& Physics2DGravityJson = sceneJson["Settings"]["Physics2DSettings"]["Gravity"];

		SceneSettings settings;
		settings.physics2DSettings.gravity.x = Physics2DGravityJson["x"];
		settings.physics2DSettings.gravity.y = Physics2DGravityJson["y"];
		Ref<Scene> scene = CreateRef<Scene>(settings);

		Vector<Entity> entities;
		Map<uint32_t, uint32_t> entityParentAssignList;
		uint32_t i = 0;
		for (auto& entityJson : sceneJson["Entities"])
		{
			Entity entity = scene->CreateEntity(entityJson["Name"]);
			entities.push_back(entity);

			auto it = entityParentAssignList.begin();
			while (it != entityParentAssignList.end())
			{
				if (it->second <= i)
				{
					entities[it->first].SetParent(entities[it->second]);
					it = entityParentAssignList.erase(it);
				}
				else
				{
					it++;
				}
			}

			// Family
			auto& base = entity.GetComponent<BaseComponent>();

			auto parentJson = entityJson["Parent"];
			if (!parentJson.is_null())
			{
				if (parentJson < i)
					entity.SetParent(entities[parentJson]);
				else
					entityParentAssignList[i] = parentJson.get<uint32_t>();
			}

			base.ID = Guid(entityJson["GUID"]);

			// Transform
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& transformJson = entityJson["Transform"];

			transform.SetPosition({
				transformJson["Position"]["x"],
				transformJson["Position"]["y"],
				transformJson["Position"]["z"]
			});

			transform.SetEulerAngles({
				transformJson["Rotation"]["x"],
				transformJson["Rotation"]["y"],
				transformJson["Rotation"]["z"]
			});

			transform.SetScale({
				transformJson["Scale"]["x"],
				transformJson["Scale"]["y"],
				transformJson["Scale"]["z"]
			});

			auto& componentsJson = entityJson["Components"];

			for (auto& componentJson : componentsJson)
			{
				if (componentJson["TypeName"] == "_CameraComponent")
				{
					if (componentJson["m_Type"] == "Orthographic")
					{
						Camera cam = Camera(
							CameraType::Orthographic, componentJson["m_OrthographicSize"],
							1, componentJson["m_ZNear"], componentJson["m_ZFar"]
						);

						cam.SetIsClearingColor(componentJson["m_IsClearingColor"]);
						cam.SetClearColor({
							componentJson["m_ClearColor"]["r"],
							componentJson["m_ClearColor"]["g"],
							componentJson["m_ClearColor"]["b"],
							componentJson["m_ClearColor"]["a"]
							});
						cam.SetIsClearingDepth(componentJson["m_IsClearingDepth"]);
						entity.AddComponent<CameraComponent>(cam);
					}
					// TODO: Perspective cameras
				}
				else if (componentJson["TypeName"] == "_SpriteRendererComponent")
				{
					entity.AddComponent<SpriteRendererComponent>(nullptr, Color{
						componentJson["m_Tint"]["r"],
						componentJson["m_Tint"]["g"],
						componentJson["m_Tint"]["b"],
						componentJson["m_Tint"]["a"]
						});
				}
			}
			i++;
		}

		return scene;
	}

	void SaveSceneToFile(const String& path, Ref<Scene> scene)
	{
		nlohmann::json sceneJson;

		// Physics2D Settings
		const auto& Physics2DGravity = scene->GetPhysicsWorld2D().GetGravity();

		sceneJson["Settings"]["Physics2DSettings"]["Gravity"] = {
			{ "x", Physics2DGravity.x },
			{ "y", Physics2DGravity.y }
		};

		Vector<uint32_t> entityIDs;
		Vector<nlohmann::json> entitiesJson;
		uint32_t size = scene->GetEntityCount();
		entityIDs.reserve(size);
		entitiesJson.reserve(size);

		scene->Each([&](Entity entity)
		{
				entityIDs.push_back(entity.GetRuntimeID());
		});

		uint32_t i = 0;
		scene->Each([&](Entity entity)
		{
			const auto& componentList = entity.GetComponentsTypeIDList();

			entitiesJson.push_back({ nlohmann::json() });
			auto& entityJson = entitiesJson[i];

			const auto& base = entity.GetComponent<BaseComponent>();

			// Name
			entityJson["Name"] = base.Name;

			// Family
			if (!base.Parent)
			{
				entityJson["Parent"] = nullptr;
			}
			else
			{
				auto it = std::find(entityIDs.begin(), entityIDs.end(), base.Parent.GetRuntimeID());
				OE_CORE_ASSERT(it != entityIDs.end(), "Parent not found!");
				entityJson["Parent"] = it - entityIDs.begin();
			}

			entityJson["GUID"] = base.ID.ToString();

			// Transform
			if (entity.HasComponent<TransformComponent>())
			{
				auto& transform = entity.GetComponent<TransformComponent>();

				entityJson["Transform"]["Position"] = {
					{ "x", transform.GetPosition().x },
					{ "y", transform.GetPosition().y },
					{ "z", transform.GetPosition().z }
				};

				entityJson["Transform"]["Rotation"] = {
					{ "x", transform.GetEulerAngles().x },
					{ "y", transform.GetEulerAngles().y },
					{ "z", transform.GetEulerAngles().z }
				};

				entityJson["Transform"]["Scale"] = {
					{ "x", transform.GetScale().x },
					{ "y", transform.GetScale().y },
					{ "z", transform.GetScale().z }
				};
			}
			else
			{
				entityJson["Transform"] = nullptr;
			}

			entityJson["Components"] = Vector<nlohmann::json>();
			for (const auto& componentTypeID : componentList)
			{
				nlohmann::json componentJson;
				bool componentParsed = false;

				if (componentTypeID == GetComponentTypeID<CameraComponent>())
				{
					auto& camera = entity.GetComponent<CameraComponent>();

					componentJson["TypeName"] = "_CameraComponent";

					if (camera.Camera.IsOrthographic())
					{
						componentJson["m_Type"] = "Orthographic";
						componentJson["m_OrthographicSize"] = camera.Camera.GetOrthographicSize();

						componentJson["m_ZNear"] = camera.Camera.GetZNear();
						componentJson["m_ZFar"] = camera.Camera.GetZFar();

						componentJson["m_IsClearingColor"] = camera.Camera.GetIsClearingColor();
						componentJson["m_ClearColor"] = {
							{ "r", camera.Camera.GetClearColor().r },
							{ "g", camera.Camera.GetClearColor().g },
							{ "b", camera.Camera.GetClearColor().b },
							{ "a", camera.Camera.GetClearColor().a }
						};
						componentJson["m_IsClearingDepth"] = camera.Camera.GetIsClearingDepth();
					}
					// TODO: Perspective cameras

					componentParsed = true;
				}
				else if (componentTypeID == GetComponentTypeID<SpriteRendererComponent>())
				{
					auto& spriteRenderer = entity.GetComponent<SpriteRendererComponent>();

					componentJson["TypeName"] = "_SpriteRendererComponent";

					componentJson["m_Tint"] = {
						{ "r", spriteRenderer.Tint.r },
						{ "g", spriteRenderer.Tint.g },
						{ "b", spriteRenderer.Tint.b },
						{ "a", spriteRenderer.Tint.a }
					};

					componentParsed = true;
				}

				if (componentParsed)
					entityJson["Components"].push_back(componentJson);
			}

			i++;
		});

		sceneJson["Entities"] = entitiesJson;

		std::ofstream sceneFile(path);
		sceneFile << sceneJson.dump(1, '\t');
		sceneFile.flush();
		sceneFile.close();
	}

}
