#pragma once

#include "OverEngine/Core/Math/Math.h"

#include "OverEngine/Renderer/Texture.h"

namespace OverEngine
{
	struct NameComponent
	{
		String Name;

		NameComponent() = default;
		NameComponent(const NameComponent&) = default;
		NameComponent(const String& name)
			: Name(name) {}
	};

	/*struct TransformComponent
	{
		Mat4x4 Transform { 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const Mat4x4& transform)
			: Transform(transform) {}

		operator Mat4x4& () { return Transform; }
		operator const Mat4x4& () const { return Transform; }
	};*/

	class TransformComponent
	{
	public:
		TransformComponent(
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
		inline void SetTransformationMatrix(const Mat4x4& transformationMatrix) { m_TransformationMatrix = transformationMatrix; }

		inline const Vector3& GetPosition() const { return m_Position; }
		inline const Quaternion& GetRotation() const { return m_Rotation; }
		inline const Vector3& GetScale() const { return m_Scale; }

		inline void SetPosition(const Vector3& position) { m_Position = position; }
		inline void SetRotation(const Quaternion& rotation) { m_Rotation = rotation; }
		inline void SetScale(const Vector3& scale) { m_Scale = scale; }

		operator Mat4x4& () { return m_TransformationMatrix; }
		operator const Mat4x4& () const { return m_TransformationMatrix; }
	private:
		Mat4x4 m_TransformationMatrix;

		Vector3 m_Position;
		Quaternion m_Rotation;
		Vector3 m_Scale;
	};

	struct SpriteRendererComponent
	{
		Ref<Texture2D> Sprite;
		Color Tint { 1.0f, 1.0f, 1.0f, 1.0f };
		float TilingFactor = 1.0f;
		bool flipX = false, flipY = false;
		TextureWrapping overrideSWrapping = TextureWrapping::None;
		TextureWrapping overrideTWrapping = TextureWrapping::None;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;

		SpriteRendererComponent(Ref<Texture2D> sprite)
			: Sprite(sprite) {}

		SpriteRendererComponent(Ref<Texture2D> sprite, const Color& tint)
			: Sprite(sprite), Tint(tint) {}
	};
}