#include "pcheader.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"

#include "OverEngine/Renderer/Renderer2D.h"
#include "OverEngine/Physics/PhysicsWorld2D.h"

namespace OverEngine
{
	Scene::Scene(const SceneSettings& settings /*= SceneSettings()*/)
		: m_PhysicsWorld2D(settings.physics2DSettings.gravity)
	{
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const String& name /*= String()*/)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<NameComponent>(name.empty() ? "Entity" : name);
		entity.AddComponent<FamilyComponent>();
		entity.AddComponent<TransformComponent>();
		m_RootEntities.push_back(entity);
		return entity;
	}

	Entity Scene::CreateEntity(Entity& parent, const String& name /*= String()*/)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<NameComponent>(name.empty() ? "Entity" : name);
		entity.AddComponent<FamilyComponent>(parent);
		return entity;
	}

	void Scene::UpdatePhysicsAndTransform(TimeStep DeltaTime)
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
			auto view = m_Registry.view<TransformComponent>();
			for (auto entity : view)
			{
				TransformComponent& transform = view.get<TransformComponent>(entity);

				if (!transform.IsChanged())
					continue;

				transform.m_Changed = false;

				transform.RecalculateTransformationMatrix();

				if (!transform.m_ChangedByPhysics && m_Registry.has<PhysicsBodyComponent>(entity))
				{
					auto& body = m_Registry.get<PhysicsBodyComponent>(entity);
					if (body.Enabled)
					{
						body.Body->SetPosition({ transform.GetPosition().x, transform.GetPosition().y });
						body.Body->SetRotation(QuaternionEulerAnglesRadians(transform.GetRotation()).z);
						transform.m_ChangedByPhysics = false;
					}
				}
			}
		}

		{
			auto group = m_Registry.group<PhysicsBodyComponent>(entt::get<TransformComponent>);
			for (auto entity : group)
			{
				auto& body = group.get<PhysicsBodyComponent>(entity);
				if (body.Enabled)
				{
					auto& transform = group.get<TransformComponent>(entity);

					transform.SetPosition(Vector3(body.Body->GetPosition(), 0));
					transform.SetRotation(QuaternionEuler(Vector3(0.0f, 0.0f, glm::degrees(body.Body->GetRotation()))));
					transform.m_ChangedByPhysics = true;
				}
			}
		}
	}

	void Scene::Render()
	{
		/////////////////////////////////////////////////////
		// Render ///////////////////////////////////////////
		/////////////////////////////////////////////////////

		auto group = m_Registry.group<TransformComponent>(entt::get<CameraComponent>);
		for (auto entity : group)
		{
			auto& transform = group.get<TransformComponent>(entity);
			auto& camera = group.get<CameraComponent>(entity);

			if (!camera.Enabled)
				continue;

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

							Renderer2D::DrawQuad(sptransform, sprite.Sprite, data);
						}
						else
						{
							Renderer2D::DrawQuad(sptransform, sprite.Tint);
						}
					}
				}
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnUpdate(TimeStep DeltaTime)
	{
		UpdatePhysicsAndTransform(DeltaTime); // TODO: use a FixedUpdate (just like Unity)
		Render();
	}
}