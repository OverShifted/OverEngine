#pragma once

#include "OverEngine/Core/Math/Math.h"
#include <entt.hpp>

#include "Components.h"

namespace OverEngine
{
	class Scene;

	class TransformComponent : public Component
	{
	public:
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(Entity& entity, Entity parent = Entity())
			: Component(entity)
		{
			if (parent)
			{
				m_Parent = parent.GetRuntimeID();
				parent.GetComponent<TransformComponent>().m_Children.push_back(entity.GetRuntimeID());
			}

			Invalidate();
		}

		inline const Mat4x4& GetLocalToWorld() const { return m_LocalToWorld; }
		inline operator const Mat4x4& () const { return GetLocalToWorld(); }

		// Position
		Vector3 GetPosition() const;
		void SetPosition(const Vector3& position);

		// Local Position
		Vector3 GetLocalPosition() const;
		void SetLocalPosition(const Vector3& position);

		// EulerAngles
		Vector3 GetEulerAngles();
		void SetEulerAngles(const Vector3& rotation);

		// Local EulerAngles
		inline const Vector3& GetLocalEulerAngles() const { return m_LocalEulerAngles; }
		void SetLocalEulerAngles(const Vector3& rotation);

		// Rotation
		Quaternion GetRotation();
		void SetRotation(const Quaternion& rotation);

		// Local Rotation
		inline const Quaternion& GetLocalRotation() const { return m_LocalRotation; }
		void SetLocalRotation(const Quaternion& rotation);

		inline const Vector3& GetLocalScale() const { return m_LocalScale; }
		void SetLocalScale(const Vector3& scale);

		Vector3 GetLossyScale() const;

		// Sets entity parent to scene
		void DetachFromParent();
		void DetachChildren();

		Entity GetParent() const;
		void SetParent(Entity parent);

		uint32_t GetSiblingIndex();
		void SetSiblingIndex(uint32_t index);

		Vector<entt::entity>& GetChildrenHandles() { return m_Children; }
		const Vector<entt::entity>& GetChildrenHandles() const { return m_Children; }

		/**
		 * Func should be void(*func)(Entity);
		 * Using template allow func to be a capturing lambda
		 */

		template <typename Func>
		void EachChild(Func func) const
		{
			for (const auto& child : m_Children)
				func({ child, AttachedEntity.GetScene() });
		}

		using ChangedFlags = int8;

		bool IsChanged() const { return m_ChangedFlags & ChangedFlags_Changed; }
		ChangedFlags GetChangedFlags() const { return m_ChangedFlags; }

		operator bool() const { return AttachedEntity; }

		bool operator==(const TransformComponent& other) const
		{
			return AttachedEntity == other.AttachedEntity;
		}

		bool operator!=(const TransformComponent& other) const
		{
			return !(*this == other);
		}
		
		static SerializationContext* Reflect()
		{
			static bool initialized = false;
			static SerializationContext ctx;

			if (!initialized)
			{
				initialized = true;

				ctx.AddField(SerializableType::Float3, OffsetOf(&TransformComponent::m_LocalToParent) + 12 * sizeof(float), "m_LocalPosition");
				ctx.AddField(SerializableType::Float3, SERIALIZE_FIELD(TransformComponent, m_LocalEulerAngles));
				ctx.AddField(SerializableType::Float3, SERIALIZE_FIELD(TransformComponent, m_LocalScale));
			}

			return &ctx;
		}

		COMPONENT_TYPE(TransformComponent)
	private:
		enum ChangedFlags_ : ChangedFlags
		{
			ChangedFlags_None = 0,
			ChangedFlags_Changed = BIT(0),
			ChangedFlags_ChangedForPhysics = BIT(1),
			// RN = Recalculation Needed
			ChangedFlags_LocalToParent_RN = BIT(2),
			ChangedFlags_LocalToWorld_RN = BIT(3)
		};

		void Invalidate();
		void Change();
	private:
		friend class Scene;

		// Use of entt::entity(uint32_t) instead of Entity
		// 1. We don't need to include Entity.h
		// 2. We don't need to store Scene* per child on the heap
		entt::entity m_Parent = entt::null;
		Vector<entt::entity> m_Children;

		// Push changes to physics in first update
		ChangedFlags m_ChangedFlags = ChangedFlags_ChangedForPhysics;

		Mat4x4 m_LocalToParent = IDENTITY_MAT4X4; // Local Matrix
		Mat4x4 m_LocalToWorld = IDENTITY_MAT4X4; // Parent's Local Matrix * Local Matrix

		Vector3 m_LocalEulerAngles = Vector3(0.0f);
		Quaternion m_LocalRotation = IDENTITY_QUATERNION;
		Vector3 m_LocalScale = Vector3(1.0f);
	};
}
