#pragma once

#include "Collider2D.h"

namespace OverEngine
{
	enum class RigidBody2DType : uint8_t { Static = 0, Kinematic, Dynamic	};

	struct RigidBody2DProps
	{
		RigidBody2DType Type = RigidBody2DType::Static;

		Vector2 Position = Vector2(0.0f, 0.0f);
		float Rotation = 0.0f;

		Vector2 LinearVelocity = Vector2(0.0f, 0.0f);
		float AngularVelocity = 0.0f;

		float LinearDamping = 0.0f;
		float AngularDamping = 0.0f;

		bool AllowSleep = false;
		bool Awake = true;
		bool Enabled = true;

		bool FixedRotation = false;
		float GravityScale = 1.0f;
		bool Bullet = false;
	};

	class PhysicWorld2D;

	class RigidBody2D
	{
	public:
		RigidBody2D(b2Body* bodyHandle);

		RigidBody2DType GetType();
		void SetType(const RigidBody2DType& type);

		Vector2 GetPosition();
		void SetPosition(const Vector2& position);

		float GetRotation();
		void SetRotation(float rotation);

		Vector2 GetLinearVelocity();
		void SetLinearVelocity(Vector2 velocity);

		float GetAngularVelocity();
		void SetAngularVelocity(float velocity);

		bool IsEnabled() const;
		void SetEnabled(bool enabled);

		Ref<Collider2D> CreateCollider(const Collider2DProps& props);
		void DestroyCollider(const Ref<Collider2D>& collider);
	private:
		b2Body* m_BodyHandle;
		Vector<Ref<Collider2D>> m_Colliders;

		friend class PhysicWorld2D;
		friend class Collider2D;
	};
}
