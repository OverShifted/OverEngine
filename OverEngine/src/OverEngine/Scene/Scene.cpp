#include "pcheader.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "TransformComponent.h"

#include "OverEngine/Renderer/Renderer2D.h"
#include "OverEngine/Physics/PhysicsWorld2D.h"
#include "OverEngine/Assets/Texture2DAsset.h"

#include "OverEngine/Core/Random.h"

#include "OverEngine/Core/Serialization/YamlConverters.h"
#include <yaml-cpp/yaml.h>
#include <fstream>


namespace OverEngine
{
	Scene::Scene(const SceneSettings& settings)
		: m_PhysicsWorld2D(settings.physics2DSettings.gravity) // TODO: Move to OnScenePlay()
	{
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const String& name, uint64_t uuid)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<NameComponent>(name.empty() ? "Entity" : name);
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		m_RootHandles.push_back(entity.GetRuntimeID());
		return entity;
	}

	Entity Scene::CreateEntity(Entity& parent, const String& name, uint64_t uuid)
	{
		OE_CORE_ASSERT(parent, "Parent is null!");
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<NameComponent>(name.empty() ? "Entity" : name);
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>(parent);
		return entity;
	}

	void Scene::OnUpdate(TimeStep deltaTime)
	{
		OnPhysicsUpdate(deltaTime); // TODO: use a FixedUpdate (just like Unity)
		OnRender();
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

	void Scene::RenderSprites()
	{
		auto spritesGroup = m_Registry.group<SpriteRendererComponent>(entt::get<TransformComponent>);
		for (auto sp : spritesGroup)
		{
			auto& sprite = spritesGroup.get<SpriteRendererComponent>(sp);
			if (sprite.Enabled)
			{
				auto& sptransform = spritesGroup.get<TransformComponent>(sp);

				if (sprite.Sprite && sprite.Sprite->GetType() != TextureType::Placeholder)
				{
					TexturedQuadExtraData data;
					data.Tint = sprite.Tint;
					data.Tiling = sprite.Tiling;
					data.Offset = sprite.Offset;
					data.Flip = sprite.Flip;
					data.Wrapping = sprite.Wrapping;
					data.Filtering = sprite.Filtering;
					data.AlphaClipThreshold = sprite.AlphaClipThreshold;
					data.TextureBorderColor = sprite.TextureBorderColor;

					Renderer2D::DrawQuad(sptransform, sprite.Sprite, data);
				}
				else
				{
					Renderer2D::DrawQuad(sptransform, sprite.Tint, sprite.AlphaClipThreshold);
				}
			}
		}
	}

	bool Scene::OnRender()
	{
		bool anyCamera = false;
		
		m_Registry.group<TransformComponent>(entt::get<CameraComponent>).each([&anyCamera, this](auto entity, auto& tc, auto& cc) {

			if (cc.Enabled && tc.Enabled)
			{
				anyCamera = true;

				RenderCommand::SetClearColor(cc.Camera.GetClearColor());
				RenderCommand::Clear(cc.Camera.GetClearFlags());

				Renderer2D::BeginScene(glm::inverse(tc.GetLocalToWorld()), cc.Camera);
				RenderSprites();
				Renderer2D::EndScene();
			}

		});

		// Returns false if nothing is rendered
		return anyCamera;
	}

	void Scene::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		m_Registry.view<CameraComponent>().each([&width, &height](CameraComponent& cc) {

			if (cc.FixedAspectRatio)
				cc.Camera.SetViewportSize(width, height);

		});
	}

	void Scene::LoadReferences(AssetCollection& assetCollection)
	{
		m_Registry.view<SpriteRendererComponent>().each([&assetCollection](SpriteRendererComponent& sp) {

			if (sp.Sprite && sp.Sprite->GetType() == TextureType::Placeholder)
			{
				const auto& pl = std::get<PlaceHolderTextureData>(sp.Sprite->GetData());

				auto asset = assetCollection.GetAsset(pl.AssetGuid);

				if (asset->GetType() == AssetType::Texture2D)
				{
					sp.Sprite = asset->GetTexture2DAsset()->GetTextures()[pl.Texture2DGuid];
				}
			}

		});
	}

	uint32_t Scene::GetEntityCount() const
	{
		return (uint32_t)m_Registry.size<IDComponent>();
	}
}
