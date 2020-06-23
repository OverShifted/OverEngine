#pragma once

#include "OverEngine/Core/Math/Math.h"

namespace OverEngine
{
	class Transform
	{
	public:
		Transform(
			const Math::Vector3& Position = Math::Vector3(0.0f),
			const Math::Quaternion& Rotation = Math::Quaternion(1.0, 0.0, 0.0, 0.0),
			const Math::Vector3& Scale = Math::Vector3(1.0f)
		);

		~Transform() = default;

		inline const Math::Mat4x4& GetTransformationMatrix() const { return m_TransformationMatrix; }
		inline void SetTransformationMatrix(const Math::Mat4x4& transformationMatrix) { m_TransformationMatrix = transformationMatrix; }

		inline const Math::Vector3& GetPosition() const { return m_Position; }
		inline const Math::Quaternion& GetRotation() const { return m_Rotation; }
		inline const Math::Vector3& GetScale() const { return m_Scale; }

		inline void SetPosition(const Math::Vector3& position) { m_Position = position; RecalculateTransformationMatrix(); }
		inline void SetRotation(const Math::Quaternion& rotation) { m_Rotation = rotation; RecalculateTransformationMatrix(); }
		inline void SetScale(const Math::Vector3& scale) { m_Scale = scale; RecalculateTransformationMatrix(); }
	private:
		void RecalculateTransformationMatrix();
	private:
		Math::Mat4x4 m_TransformationMatrix;

		Math::Vector3 m_Position;
		Math::Quaternion m_Rotation;
		Math::Vector3 m_Scale;
	};
}