#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Scene/Entity.h"

#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>

namespace OverEngine
{
	enum class CollisionShape2DType
	{
		None = 0,
		Box,
		Circle
	};

	class CollisionShape2D: public Asset
	{
	public:
		virtual CollisionShape2DType GetType() const = 0;
		virtual b2Shape* GetBox2DShape(const Mat4x4& transform) = 0;

	protected:
		Vector2 m_Offset = { 0.0f, 0.0f };
	};

	class BoxCollisionShape2D : public CollisionShape2D
	{
	public:
		static Ref<BoxCollisionShape2D> Create(const Vector2& size, const Vector2& offset = { 0.0f, 0.0f }, float rotation = 0.0f);
		BoxCollisionShape2D(const Vector2& size, const Vector2& offset, float rotation);

		virtual CollisionShape2DType GetType() const override { return CollisionShape2DType::Box; }
		virtual b2Shape* GetBox2DShape(const Mat4x4& transform) override { Invalidate(transform); return &m_Shape; }

		const Vector2& GetSize() const { return m_Size; }
		void SetSize(const Vector2& size) { m_Size = size; }

		float GetRotation() const { return m_Rotation; }
		void SetRotation(float rotation) { m_Rotation = rotation; }
	
	private:
		void Invalidate(const Mat4x4& transform);
		b2PolygonShape m_Shape;

		Vector2 m_Size;
		float m_Rotation;
	};

	class CircleCollisionShape2D : public CollisionShape2D
	{
	public:
		static Ref<CircleCollisionShape2D> Create(float radius);
		CircleCollisionShape2D(float radius);

		virtual CollisionShape2DType GetType() const override { return CollisionShape2DType::Circle; }
		virtual b2Shape* GetBox2DShape(const Mat4x4& transform) override { Invalidate(transform); return &m_Shape; }

		float GetRadius() const { return m_Radius; }
		void SetRadius(float radius) { m_Radius = radius; }

	private:
		void Invalidate(const Mat4x4& transform);
		b2CircleShape m_Shape;

		float m_Radius;
	};

	struct Collider2DProps
	{
		Entity AttachedEntity = Entity();

		Ref<CollisionShape2D> Shape = nullptr;

		// Surface Properties
		float Friction = 0.2f;
		float Bounciness = 0.0f;
		float BouncinessThreshold = 1.0f;

		// Other Properties
		float Density = 1.0f;
		bool IsTrigger = false;
	};

	class RigidBody2D;

	class Collider2D : public std::enable_shared_from_this<Collider2D>
	{
	public:
		// Creates an un-deployed collider
		static Ref<Collider2D> Create(const Collider2DProps& props);

		Collider2D(const Collider2DProps& props);
		~Collider2D();

		// Creates a Box2D fixture
		void Deploy(RigidBody2D* rigidBody);
		void UnDeploy();

		Collider2DProps& GetProps() { return m_Props; }
		const Collider2DProps& GetProps() const { return m_Props; }

		RigidBody2D* GetAttachedBody() const { return m_BodyHandle; }

		inline float GetFriction() const { return m_Props.Friction; }
		inline void SetFriction(float friction) { m_Props.Friction = friction; Invalidate(); }

		inline float GetDensity() const { return m_Props.Density; }
		inline void SetDensity(float density) { m_Props.Density = density; Invalidate(); }

		inline float GetBounciness() const{ return m_Props.Bounciness; }
		inline void SetBounciness(float bounciness) { m_Props.Bounciness = bounciness; Invalidate(); }

		inline float GetBouncinessThreshold() const { return m_Props.BouncinessThreshold; }
		inline void SetBouncinessThreshold(float bouncinessThreshold) { m_Props.BouncinessThreshold = bouncinessThreshold; Invalidate(); }

		inline bool IsTrigger() const{ return m_Props.IsTrigger; }
		inline void SetTrigger(bool isTrigger) { m_Props.IsTrigger = isTrigger; Invalidate(); }

	private:
		void Invalidate();

	private:
		Collider2DProps m_Props;

		b2Fixture* m_FixtureHandle = nullptr;
		RigidBody2D* m_BodyHandle = nullptr;
	};
}
