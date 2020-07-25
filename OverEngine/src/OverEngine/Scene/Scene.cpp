#include "pcheader.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"

#include "OverEngine/Renderer/Renderer2D.h"

namespace OverEngine
{
	Scene::Scene()
	{
		
	}

	Scene::~Scene()
	{

	}
	
	Entity Scene::CreateEntity(const String& name /*= String()*/)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& nameComponent = entity.AddComponent<NameComponent>();
		nameComponent.Name = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::OnUpdate(TimeStep ts)
	{
		auto transformView = m_Registry.view<TransformComponent>();
		for (auto entity : transformView)
		{
			TransformComponent& transform = transformView.get<TransformComponent>(entity);
			transform.RecalculateTransformationMatrix();
		}

		auto spriteEntities = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : spriteEntities)
		{
			auto& [transform, sprite] = spriteEntities.get<TransformComponent, SpriteRendererComponent>(entity);

			TexturedQuadExtraData data
			{
				sprite.Tint,
				sprite.TilingFactor,
				sprite.flipX,
				sprite.flipY,
				sprite.overrideSWrapping,
				sprite.overrideTWrapping,
			};

			Renderer2D::DrawQuad(transform, sprite.Sprite, data);
		}
	}

}