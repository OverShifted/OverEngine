#pragma once

#include "Entity.h"

#include "OverEngine/Core/Math/Math.h"
#include "OverEngine/Core/Core.h"
#include "OverEngine/Core/GUIDGenerator.h"

#include "OverEngine/Assets/Asset.h"

#include "OverEngine/Physics/PhysicsBody2D.h"
#include "OverEngine/Physics/PhysicsCollider2D.h"

#include "OverEngine/Renderer/Camera.h"

namespace OverEngine
{
	class Scene;

	enum class ComponentType
	{
		NameComponent, GUIDComponent, FamilyComponent, TransformComponent,
		CameraComponent, SpriteRendererComponent, PhysicsBody2DComponent,
		PhysicsColliders2DComponent
	};

	#define COMPONENT_TYPE(type) static ComponentType GetStaticType() { return ComponentType::type; }\
							virtual ComponentType GetComponentType() const override { return GetStaticType(); }\
							virtual const char* GetName() const override { return #type; }

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

	struct NameComponent : public Component
	{
		String Name;

		NameComponent() = default;
		NameComponent(const NameComponent&) = default;
		NameComponent(Entity& entity, const String& name)
			: Component(entity), Name(name) {}

		COMPONENT_TYPE(NameComponent)
	};

	struct GUIDComponent : public Component
	{
		Guid ID = GUIDGenerator::GenerateVersion4();

		GUIDComponent() = default;
		GUIDComponent(const GUIDComponent&) = default;
		GUIDComponent(Entity& entity)
			: Component(entity) {}
		GUIDComponent(Entity& entity, const Guid & id)
			: Component(entity), ID(id) {}

		COMPONENT_TYPE(GUIDComponent)
	};

	struct FamilyComponent : public Component
	{
		Entity Parent;
		Vector<Entity> Children;

		FamilyComponent() = default;
		FamilyComponent(const FamilyComponent&) = default;

		FamilyComponent(Entity& entity)
			: Component(entity) {}

		FamilyComponent(Entity& entity, Entity& parent)
			: Component(entity), Parent(parent)
		{
			parent.GetComponent<FamilyComponent>().Children.push_back(entity);
		}

		COMPONENT_TYPE(FamilyComponent)
	};

	struct TransformComponent : public Component
	{
	public:
		TransformComponent(
			Entity& entity,
			const Vector3& Position = Vector3(0.0f),
			const Quaternion& Rotation = Quaternion(1.0, 0.0, 0.0, 0.0),
			const Vector3& Scale = Vector3(1.0f)
		)
			: Component(entity), m_Position(Position), m_Rotation(Rotation), m_EulerAngles(Vector3(0)), m_Scale(Scale)
		{
			RecalculateTransformationMatrix();
		}

		~TransformComponent() = default;

		void RecalculateTransformationMatrix()
		{
			m_TransformationMatrix =
				glm::translate(Mat4x4(1.0), m_Position) *
				glm::mat4_cast(m_Rotation) *
				glm::scale(Mat4x4(1.0), m_Scale);
		}

		inline const Mat4x4& GetTransformationMatrix() const { return m_TransformationMatrix; }
		inline void SetTransformationMatrix(const Mat4x4& transformationMatrix)
		{
			m_TransformationMatrix = transformationMatrix;
			m_Changed = true;
			m_ChangedByPhysics = false;
		}

		inline const Vector3& GetPosition() const { return m_Position; }
		inline const Vector3& GetEulerAngles() const { return m_EulerAngles; }
		inline const Quaternion& GetRotation() const { return m_Rotation; }
		inline const Vector3& GetScale() const { return m_Scale; }

		inline void SetPosition(const Vector3& position)
		{
			m_Position = position;
			m_Changed = true;
			m_ChangedByPhysics = false;
		}

		inline void SetRotation(const Quaternion& rotation)
		{
			m_Rotation = rotation;
			m_EulerAngles = QuaternionEulerAngles(m_Rotation);
			m_Changed = true;
			m_ChangedByPhysics = false;
		}

		inline void SetEulerAngles(const Vector3& rotation)
		{
			m_EulerAngles = rotation;
			m_Rotation = QuaternionEuler(m_EulerAngles);
			m_Changed = true;
			m_ChangedByPhysics = false;
		}

		inline void SetScale(const Vector3& scale)
		{
			m_Scale = scale;
			m_Changed = true;
			m_ChangedByPhysics = false;
		}

		operator Mat4x4& () { return m_TransformationMatrix; }
		operator const Mat4x4& () const { return m_TransformationMatrix; }

		inline bool IsChanged() { return m_Changed; }

		COMPONENT_TYPE(TransformComponent)
	private:
		Vector3 m_Position;
		Vector3 m_EulerAngles;
		Quaternion m_Rotation;
		Vector3 m_Scale;

		// Runtime
		bool m_Changed = true;
		bool m_ChangedByPhysics = false;
		Mat4x4 m_TransformationMatrix;

		friend class Scene;
	};

	////////////////////////////////////////////////////////
	// Renderer Components /////////////////////////////////
	////////////////////////////////////////////////////////

	struct CameraComponent : public Component
	{
		OverEngine::Camera Camera;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;

		CameraComponent(Entity& entity, const OverEngine::Camera& camera)
			: Component(entity), Camera(camera) {}

		CameraComponent(Entity& entity)
			: Component(entity) {}

		COMPONENT_TYPE(CameraComponent)
	};

	struct SpriteRendererComponent : public Component
	{
		Ref<Texture2DAsset> Sprite;
		Color Tint{ 1.0f, 1.0f, 1.0f, 1.0f };
		float TilingFactorX = 1.0f;
		float TilingFactorY = 1.0f;
		bool FlipX = false, FlipY = false;
		TextureWrapping OverrideSWrapping = TextureWrapping::None;
		TextureWrapping OverrideTWrapping = TextureWrapping::None;
		TextureFiltering OverrideFiltering = TextureFiltering::None;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;

		SpriteRendererComponent(Entity& entity, Ref<Texture2DAsset> sprite)
			: Component(entity), Sprite(sprite) {}

		SpriteRendererComponent(Entity& entity, Ref<Texture2DAsset> sprite, const Color& tint)
			: Component(entity), Sprite(sprite), Tint(tint) {}

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
