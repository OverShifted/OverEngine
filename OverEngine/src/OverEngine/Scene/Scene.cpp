#include "pcheader.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "TransformComponent.h"

#include "OverEngine/Renderer/Renderer2D.h"
#include "OverEngine/Physics/PhysicsWorld2D.h"

#include "OverEngine/Core/Random.h"

#include "OverEngine/Core/Runtime/Serialization/YamlConverters.h"
#include <yaml-cpp/yaml.h>
#include <fstream>

namespace OverEngine
{
	Scene::Scene(const SceneSettings& settings)
		: m_PhysicsWorld2D(nullptr)
	{
	}

	Scene::Scene(Scene& other)
		: m_Registry(), m_ViewportWidth(other.m_ViewportWidth), m_ViewportHeight(other.m_ViewportHeight),
		  m_RootHandles(other.m_RootHandles), m_ComponentList(other.m_ComponentList)
	{
		const auto& reg = other.m_Registry;
		m_Registry.assign(reg.data(), reg.data() + reg.size());

		CopyComponentsFrom<NameComponent>(other);
		CopyComponentsFrom<IDComponent>(other);
		CopyComponentsFrom<TransformComponent>(other);
		CopyComponentsFrom<SpriteRendererComponent>(other);
		CopyComponentsFrom<CameraComponent>(other);
		CopyComponentsFrom<RigidBody2DComponent>(other);
		CopyComponentsFrom<Colliders2DComponent>(other);
	}

	Scene::~Scene()
	{
		m_Registry.view<RigidBody2DComponent>().each([this](auto entity, auto& rbc)
		{
			m_Registry.destroy(entity);
		});

		if (m_PhysicsWorld2D) delete m_PhysicsWorld2D;
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
		if (!m_PhysicsWorld2D)
			return;

		// Update Physics
		m_PhysicsWorld2D->OnUpdate(deltaTime, 8, 3);

		m_Registry.group<RigidBody2DComponent>(entt::get<TransformComponent>).each([](auto& rbc, auto& tc)
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

		m_Registry.view<NativeScriptsComponent>().each([&deltaTime](auto entity, auto& nsc)
		{
			for (auto& script : nsc.Scripts)
			{
				script.second.Instance->OnLateUpdate(deltaTime);
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
		if (m_PhysicsWorld2D)
			delete m_PhysicsWorld2D;

		m_PhysicsWorld2D = new PhysicsWorld2D({ 0.0, -9.8 });
		m_PhysicsWorld2D->SetOnCollisionCallbackUserData(this);

		m_PhysicsWorld2D->SetOnCollisionEnterCallback([](const Collision2D& collision, void* userData)
		{
			static_cast<Scene*>(userData)->OnCollisionEnter(collision);
		});

		m_PhysicsWorld2D->SetOnCollisionExitCallback([](const Collision2D& collision, void* userData)
		{
			static_cast<Scene*>(userData)->OnCollisionExit(collision);
		});

		// Construct RigidBodies
		m_Registry.view<RigidBody2DComponent>().each([this](entt::entity entity, auto& rbc)
		{
			auto& tc = m_Registry.get<TransformComponent>(entity);
			rbc.RigidBody->SetPosition(tc.GetPosition());
			rbc.RigidBody->SetRotation(tc.GetEulerAngles().z);
			rbc.RigidBody->GetProps().AttachedEntity = Entity{ entity, this };
			rbc.RigidBody->Deploy(m_PhysicsWorld2D);
		});

		// Construct Colliders
		m_Registry.view<Colliders2DComponent>().each([this](entt::entity entity, auto& pcc)
		{
			Ref<RigidBody2D> rb = FindAttachedBody({ entity, this });

			OE_CORE_ASSERT(rb, "Cannot find any attached RigidBody");

			for (auto& collider : pcc.Colliders)
			{
				collider->GetProps().AttachedEntity = Entity{ entity, this };
				collider->Deploy(rb.get());
			}
		});
	}

	void Scene::InitializeScripts()
	{
		// Initialize native scripts (C++)
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
				auto& transform = spritesGroup.get<TransformComponent>(sp);

				if (sprite.Sprite && !sprite.Sprite->IsReference())
				{
					TexturedQuadProps props;
					props.Tint      = sprite.Tint;
					props.Sprite    = sprite.Sprite;
					props.Tiling    = sprite.Tiling;
					props.Offset    = sprite.Offset;
					props.Flip      = sprite.Flip;
					props.ForceTile = sprite.ForceTile;

					Renderer2D::DrawQuad(transform.GetLocalToWorld(), props);
				}
				else
				{
					Renderer2D::DrawQuad(transform.GetLocalToWorld(), sprite.Tint);
				}
			}
		}
	}

	bool Scene::OnRender()
	{
		bool anyCamera = false;
		
		m_Registry.group<CameraComponent>(entt::get<TransformComponent>).each([&anyCamera, this](auto entity, auto& cc, auto& tc)
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
			if (!cc.FixedAspectRatio)
				cc.Camera.SetViewportSize(width, height);
		});
	}

	void Scene::HandleCollision(const Collision2D& collision, bool enter)
	{
		Entity entityA = collision.ColliderA->GetProps().AttachedEntity;
		Entity entityB = collision.ColliderB->GetProps().AttachedEntity;

		if (!Exists(entityA) || !Exists(entityB))
			return;

		if (entityA.HasComponent<NativeScriptsComponent>())
		{
			auto& nsc = entityA.GetComponent<NativeScriptsComponent>();

			if (enter)
			{
				for (auto& script : nsc.Scripts)
					script.second.Instance->OnCollisionEnter(collision);
			}
			else
			{
				for (auto& script : nsc.Scripts)
					script.second.Instance->OnCollisionExit(collision);
			}
		}

		if (entityB.HasComponent<NativeScriptsComponent>())
		{
			Collision2D bCollision;
			bCollision.ColliderA = collision.ColliderB;
			bCollision.ColliderB = collision.ColliderA;

			auto& nsc = entityB.GetComponent<NativeScriptsComponent>();
			
			if (enter)
			{
				for (auto& script : nsc.Scripts)
					script.second.Instance->OnCollisionEnter(bCollision);
			}
			else
			{
				for (auto& script : nsc.Scripts)
					script.second.Instance->OnCollisionExit(bCollision);
			}
		}
	}

	void Scene::OnCollisionEnter(const Collision2D& collision)
	{
		HandleCollision(collision, true);
	}

	void Scene::OnCollisionExit(const Collision2D& collision)
	{
		HandleCollision(collision, false);
	}
}
