#pragma once

#include "Entity.h"

#include "OverEngine/Core/Core.h"
#include "OverEngine/Core/Serialization/Serializer.h"
#include "OverEngine/Core/Math/Math.h"
#include "OverEngine/Core/GUIDGenerator.h"

#include "OverEngine/Assets/Asset.h"

#include "OverEngine/Physics/PhysicsBody2D.h"
#include "OverEngine/Physics/PhysicsCollider2D.h"

#include "OverEngine/Scene/SceneCamera.h"

namespace OverEngine
{
	class Scene;

	enum class ComponentType
	{
		BaseComponent, TransformComponent, CameraComponent,
		SpriteRendererComponent, PhysicsBody2DComponent,
		PhysicsColliders2DComponent
	};

	#define COMPONENT_TYPE(type) static ComponentType GetStaticType() { return ComponentType::type; }\
								virtual ComponentType GetComponentType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }\
								static const char* GetStaticName() { return #type; }

	struct Component
	{
		Component(Entity attachedEntity, bool enabled = true)
			: AttachedEntity(attachedEntity), Enabled(enabled) {}

		Entity AttachedEntity;
		bool Enabled = true;

		virtual ComponentType GetComponentType() const = 0;
		virtual const char* GetName() const = 0;
	};

	////////////////////////////////////////////////////////
	// Common Components ///////////////////////////////////
	////////////////////////////////////////////////////////

	struct BaseComponent : public Component
	{
		String Name;
		Guid ID = GUIDGenerator::GenerateVersion4();

		BaseComponent() = default;
		BaseComponent(const BaseComponent&) = default;
		BaseComponent(Entity& entity, const String& name)
			: Component(entity), Name(name) {}

		COMPONENT_TYPE(BaseComponent)
	};

	////////////////////////////////////////////////////////
	// Renderer Components /////////////////////////////////
	////////////////////////////////////////////////////////

	struct CameraComponent : public Component
	{
		SceneCamera Camera;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;

		CameraComponent(Entity& entity, const SceneCamera& camera)
			: Component(entity), Camera(camera) {}

		CameraComponent(Entity& entity)
			: Component(entity) {}

		static SerializationContext* Reflect()
		{
			static bool initialized = false;
			static SerializationContext ctx;

			if (!initialized)
			{
				initialized = true;

				auto& cameraReflection = *SceneCamera::Reflect();

				for (const auto& elem : cameraReflection.Elements)
				{
					ctx.Elements.push_back(elem);
					(ctx.Elements.end() - 1)->Offset += OffsetOf(&CameraComponent::Camera);
				}
			}

			return &ctx;
		}

		COMPONENT_TYPE(CameraComponent)
	};

	struct SpriteRendererComponent : public Component
	{
		Ref<Texture2DAsset> Sprite;
		Color Tint = Color(1);
		float TilingFactorX = 1.0f;
		float TilingFactorY = 1.0f;
		bool FlipX = false, FlipY = false;
		TextureWrapping OverrideSWrapping = TextureWrapping::None;
		TextureWrapping OverrideTWrapping = TextureWrapping::None;
		TextureFiltering OverrideFiltering = TextureFiltering::None;
		float AlphaClippingThreshold = 0.0f;
		bool IsOverridingTextureBorderColor = false;
		Color OverrideTextureBorderColor = Color(1);

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;

		SpriteRendererComponent(Entity& entity, Ref<Texture2DAsset> sprite = nullptr)
			: Component(entity), Sprite(sprite) {}

		SpriteRendererComponent(Entity& entity, Ref<Texture2DAsset> sprite, const Color& tint)
			: Component(entity), Sprite(sprite), Tint(tint) {}

		static SerializationContext* Reflect()
		{
			static bool initialized = false;
			static SerializationContext ctx;

			if (!initialized)
			{
				initialized = true;

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

				ctx.AddField(SerializableDataType::Float4, SERIALIZE_FIELD(SpriteRendererComponent, Tint));

				ctx.AddField(SerializableDataType::Float, SERIALIZE_FIELD(SpriteRendererComponent, TilingFactorX));
				ctx.AddField(SerializableDataType::Float, SERIALIZE_FIELD(SpriteRendererComponent, TilingFactorY));

				ctx.AddField(SerializableDataType::Bool, SERIALIZE_FIELD(SpriteRendererComponent, FlipX));
				ctx.AddField(SerializableDataType::Bool, SERIALIZE_FIELD(SpriteRendererComponent, FlipY));

				ctx.AddEnumField(SerializableDataType::Int8Enum, "TextureWrapping", SERIALIZE_FIELD(SpriteRendererComponent, OverrideSWrapping));
				ctx.AddEnumField(SerializableDataType::Int8Enum, "TextureWrapping", SERIALIZE_FIELD(SpriteRendererComponent, OverrideTWrapping));
				ctx.AddEnumField(SerializableDataType::Int8Enum, "TextureFiltering", SERIALIZE_FIELD(SpriteRendererComponent, OverrideFiltering));

				ctx.AddField(SerializableDataType::Float, SERIALIZE_FIELD(SpriteRendererComponent, AlphaClippingThreshold));

				ctx.AddField(SerializableDataType::Bool, SERIALIZE_FIELD(SpriteRendererComponent, IsOverridingTextureBorderColor));
				ctx.AddField(SerializableDataType::Float4, SERIALIZE_FIELD(SpriteRendererComponent, OverrideTextureBorderColor));
			}

			return &ctx;
		}

		COMPONENT_TYPE(SpriteRendererComponent)
	};

	////////////////////////////////////////////////////////
	// Physics Components //////////////////////////////////
	////////////////////////////////////////////////////////

	struct PhysicsBody2DComponent : public Component
	{
		Ref<PhysicsBody2D> Body;

		PhysicsBody2DComponent() = default;
		PhysicsBody2DComponent(const PhysicsBody2DComponent&) = default;

		PhysicsBody2DComponent(Entity& entity, const PhysicsBodyProps& props);

		COMPONENT_TYPE(PhysicsBody2DComponent)
	};

	struct PhysicsColliders2DComponent : public Component
	{
		Vector<Ref<PhysicsCollider2D>> Colliders;
		Ref<PhysicsBody2D> AttachedBody;

		PhysicsColliders2DComponent() = default;
		PhysicsColliders2DComponent(const PhysicsColliders2DComponent&) = default;

		PhysicsColliders2DComponent(Entity& entity)
			: Component(entity)
		{
			if (entity.HasComponent<PhysicsBody2DComponent>())
				AttachedBody = entity.GetComponent<PhysicsBody2DComponent>().Body;
			else
				AttachedBody = nullptr;
		}

		void AddCollider(const Ref<PhysicsCollider2D>& collider)
		{
			Colliders.push_back(collider);

			if (AttachedBody)
				AttachedBody->AddCollider(collider);
		}

		void RemoveCollider(const Ref<PhysicsCollider2D>& collider)
		{
			auto it = std::find(Colliders.begin(), Colliders.end(), collider);
			if (it != Colliders.end())
			{
				if (AttachedBody)
					AttachedBody->RemoveCollider(collider);
				Colliders.erase(it);
			}
			else
				OE_CORE_ASSERT(false, "Collider dosen't exist");
		}

		bool HasCollider(const Ref<PhysicsCollider2D>& collider)
		{
			auto it = std::find(Colliders.begin(), Colliders.end(), collider);
			return it != Colliders.end();
		}

		COMPONENT_TYPE(PhysicsColliders2DComponent)
	};
}
