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

			ctx.AddField(SerializableType::Float4, SERIALIZE_FIELD(SpriteRendererComponent, Tint));

			ctx.AddField(SerializableType::Float2, SERIALIZE_FIELD(SpriteRendererComponent, Tiling));
			ctx.AddField(SerializableType::Float2, SERIALIZE_FIELD(SpriteRendererComponent, Offset));

			ctx.AddField(SerializableType::Bool, OffsetOf(&SpriteRendererComponent::Flip), "Flip.x");
			ctx.AddField(SerializableType::Bool, OffsetOf(&SpriteRendererComponent::Flip) + sizeof(bool), "Flip.y");

			ctx.AddEnumField(SerializableType::Int8Enum, "TextureWrapping", OffsetOf(&SpriteRendererComponent::Wrapping), "Wrapping.x");
			ctx.AddEnumField(SerializableType::Int8Enum, "TextureWrapping", OffsetOf(&SpriteRendererComponent::Wrapping) + sizeof(TextureWrapping), "Wrapping.y");
			ctx.AddEnumField(SerializableType::Int8Enum, "TextureFiltering", SERIALIZE_FIELD(SpriteRendererComponent, Filtering));

			ctx.AddField(SerializableType::Float, SERIALIZE_FIELD(SpriteRendererComponent, AlphaClipThreshold));

			ctx.AddField(SerializableType::Bool, OffsetOf(&SpriteRendererComponent::TextureBorderColor) + OffsetOf(&std::pair<bool, Color>::first), "IsOverridingTextureBorderColor");
			ctx.AddField(SerializableType::Float4, OffsetOf(&SpriteRendererComponent::TextureBorderColor) + OffsetOf(&std::pair<bool, Color>::second), "TextureBorderColor");
		}

		return &ctx;
	}

	SerializationContext* CameraComponent::Reflect()
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

	SerializationContext* RigidBody2DComponent::Reflect()
	{
		static bool initialized = false;
		static SerializationContext ctx;

		if (!initialized)
		{
			initialized = true;

			if (!Serializer::GlobalEnumExists("RigidBody2DType"))
			{
				Serializer::DefineGlobalEnum("RigidBody2DType", {
					{ 0, "Static" },
					{ 1, "Kinematic" },
					{ 2, "Dynamic" }
				});
			}

			auto offset = OffsetOf(&RigidBody2DComponent::Initializer);

			ctx.AddEnumField(SerializableType::UInt8Enum, "RigidBody2DType", offset + OffsetOf(&RigidBody2DProps::Type), "Type");

			ctx.AddField(SerializableType::Float2, offset + OffsetOf(&RigidBody2DProps::LinearVelocity), "LinearVelocity");
			ctx.AddField(SerializableType::Float, offset + OffsetOf(&RigidBody2DProps::AngularVelocity), "AngularVelocity");

			ctx.AddField(SerializableType::Float, offset + OffsetOf(&RigidBody2DProps::LinearDamping), "LinearDamping");
			ctx.AddField(SerializableType::Float, offset + OffsetOf(&RigidBody2DProps::AngularDamping), "AngularDamping");

			ctx.AddField(SerializableType::Bool, offset + OffsetOf(&RigidBody2DProps::AllowSleep), "AllowSleep");
			ctx.AddField(SerializableType::Bool, offset + OffsetOf(&RigidBody2DProps::Awake), "Awake");

			ctx.AddField(SerializableType::Bool, offset + OffsetOf(&RigidBody2DProps::FixedRotation), "FixedRotation");
			ctx.AddField(SerializableType::Float, offset + OffsetOf(&RigidBody2DProps::GravityScale), "GravityScale");
			ctx.AddField(SerializableType::Bool, offset + OffsetOf(&RigidBody2DProps::Bullet), "Bullet");
		}

		return &ctx;
	}
}
