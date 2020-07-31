#include "pcheader.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"

#include "OverEngine/Renderer/Renderer2D.h"
#include "OverEngine/Physics/PhysicsWorld2D.h"

namespace OverEngine
{
	Scene::Scene(const SceneSettings& settings /*= SceneSettings()*/)
		: m_PhysicsWorld(settings.physics2DSettings.gravity)
	{
	}

	Scene::~Scene()
	{

	}

	Entity Scene::CreateEntity(const String& name /*= String()*/)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<NameComponent>(name.empty() ? "Entity" : name);
		entity.AddComponent<FamilyComponent>();
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

	void Scene::OnUpdate(TimeStep DeltaTime)
	{
		m_PhysicsWorld.OnUpdate(DeltaTime, 8, 3);

		auto transformView = m_Registry.view<TransformComponent>();
		for (auto entity : transformView)
		{
			TransformComponent& transform = transformView.get<TransformComponent>(entity);

			if (!transform.IsChanged())
				continue;

			transform.m_Changed = false;

			transform.RecalculateTransformationMatrix();
			
			if (m_Registry.has<PhysicsBodyComponent>(entity) && !transform.m_ChangedByPhysics)
			{
				auto& body = m_Registry.get<PhysicsBodyComponent>(entity);
				body.Body->SetPosition({ transform.GetPosition().x, transform.GetPosition().y });
				body.Body->SetRotation(QuaternionEulerAngles(transform.GetRotation()).z);
				transform.m_ChangedByPhysics = false;
			}
		}

		entt::get<PhysicsBodyComponent>;
		auto bodyEntities = m_Registry.group<PhysicsBodyComponent>(entt::get<TransformComponent>);
		for (auto entity : bodyEntities)
		{
			auto& transform = bodyEntities.get<TransformComponent>(entity);
			auto& body = bodyEntities.get<PhysicsBodyComponent>(entity);

			transform.SetPosition(Vector3(body.Body->GetPosition(), 0));
			transform.SetRotation(QuaternionEuler(Vector3(0.0f, 0.0f, glm::degrees(body.Body->GetRotation()))));
			transform.m_ChangedByPhysics = true;
		}

		auto spriteEntities = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : spriteEntities)
		{
			auto& transform = spriteEntities.get<TransformComponent>(entity);
			auto& sprite = spriteEntities.get<SpriteRendererComponent>(entity);

			TexturedQuadExtraData data;
			data.tint = sprite.Tint;
			data.tilingFactorX = sprite.TilingFactorX;
			data.tilingFactorY = sprite.TilingFactorY;
			data.flipX = sprite.FlipX;
			data.flipY = sprite.FlipY;
			data.overrideSTextureWrapping = sprite.overrideSWrapping;
			data.overrideTTextureWrapping = sprite.overrideTWrapping;

			Renderer2D::DrawQuad(transform, sprite.Sprite, data);
		}
	}
}