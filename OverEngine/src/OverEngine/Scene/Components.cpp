#include "pcheader.h"
#include "Components.h"

namespace OverEngine
{
	SerializationContext* SpriteRendererComponent::Reflect()
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

			ctx.AddField(SerializableDataType::Float2, SERIALIZE_FIELD(SpriteRendererComponent, Tiling));
			ctx.AddField(SerializableDataType::Float2, SERIALIZE_FIELD(SpriteRendererComponent, Offset));

			ctx.AddField(SerializableDataType::Bool, OffsetOf(&SpriteRendererComponent::Flip), "Flip.x");
			ctx.AddField(SerializableDataType::Bool, OffsetOf(&SpriteRendererComponent::Flip) + sizeof(bool), "Flip.y");

			ctx.AddEnumField(SerializableDataType::Int8Enum, "TextureWrapping", OffsetOf(&SpriteRendererComponent::Wrapping), "Wrapping.x");
			ctx.AddEnumField(SerializableDataType::Int8Enum, "TextureWrapping", OffsetOf(&SpriteRendererComponent::Wrapping) + sizeof(TextureWrapping), "Wrapping.y");
			ctx.AddEnumField(SerializableDataType::Int8Enum, "TextureFiltering", SERIALIZE_FIELD(SpriteRendererComponent, Filtering));

			ctx.AddField(SerializableDataType::Float, SERIALIZE_FIELD(SpriteRendererComponent, AlphaClipThreshold));

			ctx.AddField(SerializableDataType::Bool, OffsetOf(&SpriteRendererComponent::TextureBorderColor), "IsOverridingTextureBorderColor");
			ctx.AddField(SerializableDataType::Float4, OffsetOf(&SpriteRendererComponent::TextureBorderColor) + sizeof(bool), "TextureBorderColor");
		}

		return &ctx;
	}

	PhysicsBody2DComponent::PhysicsBody2DComponent(Entity& entity, const PhysicsBodyProps& props)
		: Component(entity), Body(CreateRef<PhysicsBody2D>(entity.GetScene()->GetPhysicsWorld2D(), props))
	{
		if (entity.HasComponent<PhysicsColliders2DComponent>())
		{
			auto& colliders = entity.GetComponent<PhysicsColliders2DComponent>();
			colliders.AttachedBody = Body;

			for (const auto& collider : colliders.Colliders)
			{
				Body->AddCollider(collider);
			}
		}
	}
}
