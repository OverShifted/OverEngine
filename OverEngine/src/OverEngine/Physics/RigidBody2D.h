#pragma once

#include "Collider2D.h"

namespace OverEngine
{
	enum class RigidBody2DType : uint8_t
	{
		None = 0,
		Static,
		Kinematic,
		Dynamic
	};

	struct RigidBody2DProps
	{
		RigidBody2DType Type = RigidBody2DType::Static;

		// If you've used `GetProps` method on `RigidBody2D`; then contents of the `Dynamics` struct is storing the initial value. Not the real-time one
		struct {
			Vector2 Position = Vector2(0.0f, 0.0f);
			float Rotation = 0.0f;

			Vector2 LinearVelocity = Vector2(0.0f, 0.0f);
			float AngularVelocity = 0.0f;

			bool IsAwake = true;
		} Dynamics;

		Entity AttachedEntity = Entity();

		float LinearDamping = 0.0f;
		float AngularDamping = 0.0f;

		bool Enabled = true;
		bool AllowSleep = false;

		bool FixedRotation = false;
		float GravityScale = 1.0f;
		bool Bullet = false;
	};

	class PhysicsWorld2D;

	class RigidBody2D : public std::enable_shared_from_this<RigidBody2D>
	{
	public:
		// Creates an un-deployed collider
		static Ref<RigidBody2D> Create(const RigidBody2DProps& props);

		RigidBody2D(const RigidBody2DProps& props);
		~RigidBody2D();

		// Creates a Box2D body
		void Deploy(PhysicsWorld2D* world);
		void UnDeploy(bool onDestruct = false);
		bool IsDeployed() const { return m_BodyHandle && m_WorldHandle; }

		bool IsEnabled() const;
		void SetEnabled(bool enabled);

		RigidBody2DType GetType() const;
		void SetType(const RigidBody2DType& type);

		RigidBody2DProps& GetProps() { return m_Props; }
		const RigidBody2DProps& GetProps() const { return m_Props; }

		Vector2 GetPosition() const;
		void SetPosition(const Vector2& position);

		float GetRotation() const;
		void SetRotation(float rotation);

		Vector2 GetLinearVelocity() const;
		void SetLinearVelocity(const Vector2& velocity);

		float GetAngularVelocity() const;
		void SetAngularVelocity(float velocity);

		bool IsAwake() const;
		void SetAwake(bool isAwake);

		void ApplyLinearImpulse(const Vector2& impulse, const Vector2& point, bool wake = true);
		void ApplyLinearImpulseToCenter(const Vector2& impulse, bool wake = true);

	private:
		RigidBody2DProps m_Props;

		b2Body* m_BodyHandle = nullptr;
		PhysicsWorld2D* m_WorldHandle = nullptr;
		Vector<Ref<Collider2D>> m_Colliders;

		friend class PhysicsWorld2D;
		friend class Collider2D;
	};
}
