#pragma once

#include "Entity.h"

#include "OverEngine/Core/Math/Math.h"
#include "OverEngine/Core/Core.h"

#include "OverEngine/Renderer/Texture.h"

#include "OverEngine/Physics/PhysicsBody2D.h"
#include "OverEngine/Physics/PhysicsCollider2D.h"

#include "OverEngine/Renderer/Camera.h"

namespace OverEngine
{
	class Scene;

	struct Component
	{
		bool Enabled;
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
			: Name(name) {}
	};

	struct FamilyComponent : public Component
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

	struct TransformComponent : public Component
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
		Vector3 m_Position;
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

	struct SpriteRendererComponent : public Component
	{
		Ref<Texture2D> Sprite;
		Color Tint { 1.0f, 1.0f, 1.0f, 1.0f };
		float TilingFactorX = 1.0f;
		float TilingFactorY = 1.0f;
		bool FlipX = false, FlipY = false;
		TextureWrapping OverrideSWrapping = TextureWrapping::None;
		TextureWrapping OverrideTWrapping = TextureWrapping::None;

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

	struct PhysicsBodyComponent : public Component
	{
		Ref<PhysicsBody2D> Body;

		PhysicsBodyComponent() = default;
		PhysicsBodyComponent(const PhysicsBodyComponent&) = default;

		PhysicsBodyComponent(Entity& entity, const PhysicsBodyProps& props);
	};

	struct PhysicsColliders2DComponent : public Component
	{
		Vector<Ref<PhysicsCollider2D>> Colliders;
		Ref<PhysicsBody2D> AttachedBody;

		PhysicsColliders2DComponent() = default;
		PhysicsColliders2DComponent(const PhysicsColliders2DComponent&) = default;

		PhysicsColliders2DComponent(Entity& entity)
		{
			PhysicsColliders2DComponent(); // TODO: What is this?
			if (entity.HasComponent<PhysicsBodyComponent>())
				AttachedBody = entity.GetComponent<PhysicsBodyComponent>().Body;
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
	};

	struct CameraComponent : public Component
	{
		OverEngine::Camera Camera;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;

		CameraComponent(Entity& entity, const OverEngine::Camera& camera)
			: Camera(camera)
		{
		}

		CameraComponent(Entity& entity)
		{
		}
	};
}