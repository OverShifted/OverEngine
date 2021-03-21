#pragma once

#include <box2d/b2_fixture.h>

namespace OverEngine
{
	enum class Collider2DType
	{
		Box = 0, Circle
	};

	struct Collider2DProps
	{
		Vector2 Offset{ 0.0f, 0.0f };
		float Rotation = 0.0f;

		struct
		{
			Collider2DType Type = Collider2DType::Box;
			
			Vector2 BoxSize{ 1.0f, 1.0f };
			float CircleRadius = 0.5f;
		} Shape;

		bool IsTrigger = false;

		float Friction = 0.2f;
		float Density = 1.0f;

		float Bounciness = 0.0f;
		float BouncinessThreshold = 1.0f;
	};

	class RigidBody2D;

	class Collider2D
	{
	public:
		Collider2D(RigidBody2D* body, b2Fixture* fixture, Collider2DType type, Vector2 offset, float rotation, Vector2 sizeHint);

		RigidBody2D* GetAttachedBody() const { return m_Body; }

		void ReShape(Vector2 size);
		void ReShape(float radius);

		inline const Vector2& GetOffset() { return m_Offset; }
		void SetOffset(const Vector2& offset);

		inline float GetRotation() { return m_Rotation; }
		void SetRotation(float rotation);

		const Vector2& GetSizeHint() const { return m_SizeHint; }
	protected:
		Collider2DType m_Type;
		RigidBody2D* m_Body;
		b2Fixture* m_FixtureHandle;

		Vector2 m_Offset;
		float m_Rotation;

		// if CircleCollider -> { r, <useless> }
		// if BoxCollider -> size of box
		Vector2 m_SizeHint;


		friend class RigidBody2D;
	};
}
