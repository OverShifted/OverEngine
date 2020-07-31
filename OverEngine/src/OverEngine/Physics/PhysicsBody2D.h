#pragma once

#include "OverEngine/Physics/PhysicsWorld2D.h"
#include "OverEngine/Physics/PhysicsBody2D.h"
#include "OverEngine/Physics/PhysicsCollider2D.h"

namespace OverEngine
{
	enum class PhysicsBodyType { Static, Dynamic, Kinematic };

	struct PhysicsBodyProps
	{
		PhysicsBodyProps()
		{
			Type = PhysicsBodyType::Static;

			Position = Vector2(0.0f, 0.0f);
			Rotation = 0.0f;

			LinearVelocity = Vector2(0.0f, 0.0f);
			AngularVelocity = 0.0f;

			LinearDamping = 0.0f;
			AngularDamping = 0.0f;

			AllowSleep = true;
			Awake = true;
			Enabled = true;

			FixedRotation = false;
			GravityScale = 1.0f;
			Bullet = false;
		}

		PhysicsBodyType Type;

		Vector2 Position;
		float Rotation;

		Vector2 LinearVelocity;
		float AngularVelocity;

		float LinearDamping;
		float AngularDamping;

		bool AllowSleep;
		bool Awake;
		bool Enabled;

		bool FixedRotation;
		float GravityScale;
		bool Bullet;
	};

	class PhysicsBody2D
	{
	public:
		PhysicsBody2D(PhysicsWorld2D& world, const PhysicsBodyProps& props = PhysicsBodyProps());

		Vector2 GetPosition();
		float GetRotation();

		void SetPosition(const Vector2& position);
		void SetRotation(float rotation);

		void AddCollider(const Ref<PhysicsCollider2D>& collider);
		void RemoveCollider(const Ref<PhysicsCollider2D>& collider);
	private:
		b2Body* m_BodyHandle;
		UnorderedMap<Ref<PhysicsCollider2D>, b2Fixture*> m_ColliderToFixture;
	};
}