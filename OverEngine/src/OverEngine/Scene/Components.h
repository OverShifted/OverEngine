#pragma once

#include "Entity.h"

#include "OverEngine/Core/Math/Math.h"
#include "OverEngine/Core/Core.h"

#include "OverEngine/Renderer/Texture.h"

#include "OverEngine/Physics/PhysicsBody2D.h"
#include "OverEngine/Physics/PhysicsCollider2D.h"

namespace OverEngine
{
	class Scene;

	////////////////////////////////////////////////////////
	// Common Components ///////////////////////////////////
	////////////////////////////////////////////////////////

	struct NameComponent
	{
		String Name;

		NameComponent() = default;
		NameComponent(const NameComponent&) = default;
		NameComponent(Entity& entity, const String& name)
			: Name(name) {}
	};

	struct FamilyComponent
	{
		Entity* This;
		Entity* Parent; // If Entity is root Entity, this value should be nullptr
		Vector<Entity*> Children;

		FamilyComponent() = default;
		FamilyComponent(const FamilyComponent&) = default;

		FamilyComponent(Entity& entity)
			: This(&entity), Parent(nullptr)
		{
		}

		FamilyComponent(Entity& entity, Entity& parent)
			: This(&entity), Parent(&parent)
		{
			parent.GetComponent<FamilyComponent>().Children.push_back(This);
		}
	};

	class TransformComponent
	{
	public:
		TransformComponent(
			Entity& entity,
			const Vector3& Position = Vector3(0.0f),
			const Quaternion& Rotation = Quaternion(1.0, 0.0, 0.0, 0.0),
			const Vector3& Scale = Vector3(1.0f)
		)
			: m_Position(Position), m_Rotation(Rotation), m_Scale(Scale)
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
		}

		inline const Vector3& GetPosition() const { return m_Position; }
		inline const Quaternion& GetRotation() const { return m_Rotation; }
		inline const Vector3& GetScale() const { return m_Scale; }

		inline void SetPosition(const Vector3& position)
		{
			m_Position = position;
			m_Changed = true;
		}

		inline void SetRotation(const Quaternion& rotation)
		{
			m_Rotation = rotation;
			m_Changed = true;
		}

		inline void SetScale(const Vector3& scale)
		{
			m_Scale = scale;
			m_Changed = true;
		}

		operator Mat4x4& () { return m_TransformationMatrix; }
		operator const Mat4x4& () const { return m_TransformationMatrix; }

		inline bool IsChanged() { return m_Changed; }
	private:
		Mat4x4 m_TransformationMatrix;
		Vector3 m_Position;
		Quaternion m_Rotation;
		Vector3 m_Scale;

		bool m_Changed = true;
		bool m_ChangedByPhysics = false;

		friend class Scene;
	};

	////////////////////////////////////////////////////////
	// Renderer Components /////////////////////////////////
	////////////////////////////////////////////////////////

	struct SpriteRendererComponent
	{
		Ref<Texture2D> Sprite;
		Color Tint { 1.0f, 1.0f, 1.0f, 1.0f };
		float TilingFactorX = 1.0f;
		float TilingFactorY = 1.0f;
		bool FlipX = false, FlipY = false;
		TextureWrapping overrideSWrapping = TextureWrapping::None;
		TextureWrapping overrideTWrapping = TextureWrapping::None;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;

		SpriteRendererComponent(Entity& entity, Ref<Texture2D> sprite)
			: Sprite(sprite) {}

		SpriteRendererComponent(Entity& entity, Ref<Texture2D> sprite, const Color& tint)
			: Sprite(sprite), Tint(tint) {}
	};

	////////////////////////////////////////////////////////
	// Physics Components //////////////////////////////////
	////////////////////////////////////////////////////////

	struct PhysicsBodyComponent
	{
		Ref<PhysicsBody2D> Body;

		PhysicsBodyComponent() = default;
		PhysicsBodyComponent(const PhysicsBodyComponent&) = default;

		PhysicsBodyComponent(Entity& entity, const PhysicsBodyProps& props);
	};

	struct PhysicsColliders2DComponent
	{
		Vector<Ref<PhysicsCollider2D>> Colliders;
		PhysicsBodyComponent* AttachedBody;

		PhysicsColliders2DComponent() = default;
		PhysicsColliders2DComponent(const PhysicsColliders2DComponent&) = default;

		PhysicsColliders2DComponent(Entity& entity)
		{
			PhysicsColliders2DComponent();
			if (entity.HasComponent<PhysicsBodyComponent>())
				AttachedBody = &entity.GetComponent<PhysicsBodyComponent>();
			else
				AttachedBody = nullptr;
		}

		void AddCollider(const Ref<PhysicsCollider2D>& collider)
		{
			Colliders.push_back(collider);

			if (AttachedBody)
				AttachedBody->Body->AddCollider(collider);
		}

		void RemoveCollider(const Ref<PhysicsCollider2D>& collider)
		{
			auto it = std::find(Colliders.begin(), Colliders.end(), collider);
			if (it != Colliders.end())
			{
				if (AttachedBody)
					AttachedBody->Body->RemoveCollider(collider);
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
	};
}