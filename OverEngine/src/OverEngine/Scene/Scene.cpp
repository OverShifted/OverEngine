#include "pcheader.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "TransformComponent.h"
#include "SceneCollision2D.h"

#include "OverEngine/Renderer/Renderer2D.h"
#include "OverEngine/Physics/PhysicWorld2D.h"
#include "OverEngine/Assets/Texture2DAsset.h"

#include "OverEngine/Core/Random.h"

#include "OverEngine/Core/Serialization/YamlConverters.h"
#include <yaml-cpp/yaml.h>
#include <fstream>


namespace OverEngine
{
	Scene::Scene(const SceneSettings& settings)
		: m_PhysicWorld2D(nullptr)
	{
	}

	template<typename T>
	void CopyComponents(entt::registry& src, entt::registry& dst, Scene* dstScene)
	{
		// Copy
		auto view = src.view<T>();
		dst.insert<T>(view.data(), view.data() + view.size(), view.raw(), view.raw() + view.size());

		dst.view<T>().each([&dstScene](entt::entity entity, T& component)
		{
			component.AttachedEntity = { entity, dstScene };
		});
	}

	#define CopyComponents(T) CopyComponents<T>(other.m_Registry, m_Registry, this);

	Scene::Scene(Scene& other)
		: m_Registry(), m_ViewportWidth(other.m_ViewportWidth), m_ViewportHeight(other.m_ViewportHeight),
		  m_RootHandles(other.m_RootHandles), m_ComponentList(other.m_ComponentList)
	{
		const auto& reg = other.m_Registry;
		m_Registry.assign(reg.data(), reg.data() + reg.size());

		CopyComponents(NameComponent);
		CopyComponents(IDComponent);
		CopyComponents(TransformComponent);
		CopyComponents(SpriteRendererComponent);
		CopyComponents(CameraComponent);
		CopyComponents(RigidBody2DComponent);
		CopyComponents(Colliders2DComponent);
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
		OnScriptsUpdate(deltaTime);
		OnRender();
	}

	void Scene::OnPhysicsUpdate(TimeStep deltaTime)
	{
		if (!m_PhysicWorld2D)
			return;

		/////////////////////////////////////////////////////
		// Physics & Transform //////////////////////////////
		/////////////////////////////////////////////////////

		// Update Physics
		m_PhysicWorld2D->OnUpdate(deltaTime, 8, 3);

		m_Registry.view<RigidBody2DComponent, TransformComponent>().each([](auto& rbc, auto& tc)
		{
			if (rbc.RigidBody)
			{
				rbc.RigidBody->SetEnabled(rbc.Enabled);

				if (rbc.Enabled)
				{
					if (tc.m_ChangedFlags & TransformComponent::ChangedFlags_ChangedForPhysics)
					{
						// Push changes to Box2D world
						const auto& pos = tc.GetPosition();
						rbc.RigidBody->SetPosition({ pos.x, pos.y });
						rbc.RigidBody->SetRotation(glm::radians(tc.GetEulerAngles().z));
					}
					else
					{
						// Push changes to OverEngine transform system
						tc.SetPosition(Vector3(rbc.RigidBody->GetPosition(), tc.GetPosition().z));
						const auto& angles = tc.GetEulerAngles();
						tc.SetEulerAngles({ angles.x, angles.y, glm::degrees(rbc.RigidBody->GetRotation()) });
					}

					// In both cases; we need to perform this
					// 1. we've pushed the changes to physics system and we need to remove the flag
					// 2. we've pushed the changes to OverEngine's transform system and it added the
					//    flag which we don't want
					tc.m_ChangedFlags ^= TransformComponent::ChangedFlags_ChangedForPhysics;
				}
			}
		});
	}

	void Scene::OnScriptsUpdate(TimeStep deltaTime)
	{
		m_Registry.view<NativeScriptsComponent>().each([&deltaTime](auto entity, auto& nsc)
		{
			for (auto& script : nsc.Scripts)
			{
				script.second.Instance->OnUpdate(deltaTime);
			}
		});
	}

	void Scene::OnScenePlay()
	{
		InitializePhysics();
		InitializeScripts();
	}

	static Ref<RigidBody2D> FindAttachedBody(Entity entity)
	{
		if (entity.HasComponent<RigidBody2DComponent>())
			return entity.GetComponent<RigidBody2DComponent>().RigidBody;

		if (entity.HasComponent<TransformComponent>())
			if (auto parent = entity.GetComponent<TransformComponent>().GetParent())
				return FindAttachedBody(parent);

		return nullptr;
	}

	void Scene::InitializePhysics()
	{
		if (m_PhysicWorld2D)
			delete m_PhysicWorld2D;

		m_PhysicWorld2D = new PhysicWorld2D({ 0.0, -9.8 });
		m_PhysicWorld2D->SetOnCollisionCallbackUserData(this);

		m_PhysicWorld2D->SetOnCollisionEnterCallback([](const Collision2D& collision, void* userData)
		{
			static_cast<Scene*>(userData)->OnCollisionEnter(collision);
		});

		m_PhysicWorld2D->SetOnCollisionExitCallback([](const Collision2D& collision, void* userData)
		{
			static_cast<Scene*>(userData)->OnCollisionExit(collision);
		});

		// Construct RigidBodies
		m_Registry.view<RigidBody2DComponent>().each([this](entt::entity entity, auto& rbc)
		{
			auto& tc = m_Registry.get<TransformComponent>(entity);
			rbc.RigidBody = m_PhysicWorld2D->CreateRigidBody(rbc.Initializer);
			rbc.RigidBody->UserData = (void*)entity;
			rbc.RigidBody->SetPosition(tc.GetPosition());
			rbc.RigidBody->SetRotation(tc.GetEulerAngles().z);
		});

		// Construct self-attached Colliders
		m_Registry.view<Colliders2DComponent>().each([this](entt::entity entity, auto& pcc)
		{
			Ref<RigidBody2D> rb = FindAttachedBody({ entity, this });
			for (auto& collider : pcc.Colliders)
			{
				collider.Collider = rb->CreateCollider(collider.Initializer);
			}
		});
	}

	void Scene::InitializeScripts()
	{
		m_Registry.view<NativeScriptsComponent>().each([this](auto entity, auto& nsc)
		{
			nsc.Runtime = true;

			for (auto& script : nsc.Scripts)
			{
				script.second.Instance = script.second.InstantiateScript();
				script.second.Instance->AttachedEntity = Entity{ entity, this };
				script.second.Instance->OnCreate();
			}
		});
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
					TexturedQuadProps props;
					props.Tint      = sprite.Tint;
					props.Texture   = sprite.Sprite;
					props.Tiling    = sprite.Tiling;
					props.Offset    = sprite.Offset;
					props.Flip      = sprite.Flip;
					props.Wrapping  = sprite.Wrapping;
					props.Filtering = sprite.Filtering;

					Renderer2D::DrawQuad(sptransform, props);
				}
				else
				{
					Renderer2D::DrawQuad(sptransform, sprite.Tint);
				}
			}
		}
	}

	bool Scene::OnRender()
	{
		bool anyCamera = false;
		
		m_Registry.group<TransformComponent>(entt::get<CameraComponent>).each([&anyCamera, this](auto entity, auto& tc, auto& cc)
		{
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

		m_Registry.view<CameraComponent>().each([&width, &height](CameraComponent& cc)
		{
			if (cc.FixedAspectRatio)
				cc.Camera.SetViewportSize(width, height);
		});
	}

	void Scene::LoadReferences(AssetCollection& assetCollection)
	{
		m_Registry.view<SpriteRendererComponent>().each([&assetCollection](SpriteRendererComponent& sp)
		{
			if (sp.Sprite && sp.Sprite->GetType() == TextureType::Placeholder)
			{
				const auto& pl = std::get<PlaceHolderTextureData>(sp.Sprite->GetData());
				auto asset = assetCollection.GetAsset(pl.AssetGuid);

				if (asset && asset->GetType() == AssetType::Texture2D)
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

	void Scene::OnCollisionEnter(const Collision2D& collision)
	{
		Entity entityA{ (entt::entity)(uintptr_t)collision.ColliderA->GetAttachedBody()->UserData, this };
		Entity entityB{ (entt::entity)(uintptr_t)collision.ColliderB->GetAttachedBody()->UserData, this };

		if (!Exists(entityA) || !Exists(entityB))
			return;

		if (entityA.HasComponent<NativeScriptsComponent>())
		{
			SceneCollision2D sceneCollision;
			sceneCollision.This.Entity = entityA;
			sceneCollision.This.RigidBody = entityA.GetComponent<RigidBody2DComponent>().RigidBody;
			sceneCollision.This.Collider = collision.ColliderA;

			sceneCollision.Other.Entity = entityB;
			sceneCollision.Other.RigidBody = entityB.GetComponent<RigidBody2DComponent>().RigidBody;
			sceneCollision.Other.Collider = collision.ColliderB;

			auto& nsc = entityA.GetComponent<NativeScriptsComponent>();
			for (auto& script : nsc.Scripts)
			{
				script.second.Instance->OnCollisionEnter(sceneCollision);
			}
		}

		if (entityB.HasComponent<NativeScriptsComponent>())
		{
			SceneCollision2D sceneCollision;
			sceneCollision.This.Entity = entityB;
			sceneCollision.This.RigidBody = entityB.GetComponent<RigidBody2DComponent>().RigidBody;
			sceneCollision.This.Collider = collision.ColliderB;

			sceneCollision.Other.Entity = entityA;
			sceneCollision.Other.RigidBody = entityA.GetComponent<RigidBody2DComponent>().RigidBody;
			sceneCollision.Other.Collider = collision.ColliderA;

			auto& nsc = entityB.GetComponent<NativeScriptsComponent>();
			for (auto& script : nsc.Scripts)
			{
				script.second.Instance->OnCollisionEnter(sceneCollision);
			}
		}
	}

	void Scene::OnCollisionExit(const Collision2D& collision)
	{
		Entity entityA{ (entt::entity)(uintptr_t)collision.ColliderA->GetAttachedBody()->UserData, this };
		Entity entityB{ (entt::entity)(uintptr_t)collision.ColliderB->GetAttachedBody()->UserData, this };

		if (!Exists(entityA) || !Exists(entityB))
			return;

		if (entityA.HasComponent<NativeScriptsComponent>())
		{
			SceneCollision2D sceneCollision;
			sceneCollision.This.Entity = entityA;
			sceneCollision.This.RigidBody = entityA.GetComponent<RigidBody2DComponent>().RigidBody;
			sceneCollision.This.Collider = collision.ColliderA;

			sceneCollision.Other.Entity = entityB;
			sceneCollision.Other.RigidBody = entityB.GetComponent<RigidBody2DComponent>().RigidBody;
			sceneCollision.Other.Collider = collision.ColliderB;

			auto& nsc = entityA.GetComponent<NativeScriptsComponent>();
			for (auto& script : nsc.Scripts)
			{
				script.second.Instance->OnCollisionExit(sceneCollision);
			}
		}

		if (entityB.HasComponent<NativeScriptsComponent>())
		{
			SceneCollision2D sceneCollision;
			sceneCollision.This.Entity = entityB;
			sceneCollision.This.RigidBody = entityB.GetComponent<RigidBody2DComponent>().RigidBody;
			sceneCollision.This.Collider = collision.ColliderB;

			sceneCollision.Other.Entity = entityA;
			sceneCollision.Other.RigidBody = entityA.GetComponent<RigidBody2DComponent>().RigidBody;
			sceneCollision.Other.Collider = collision.ColliderA;

			auto& nsc = entityB.GetComponent<NativeScriptsComponent>();
			for (auto& script : nsc.Scripts)
			{
				script.second.Instance->OnCollisionExit(sceneCollision);
			}
		}
	}
}
