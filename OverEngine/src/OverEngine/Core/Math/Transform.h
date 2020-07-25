#pragma once

#include "OverEngine/Core/Math/Math.h"

namespace OverEngine
{
	class Transform
	{
	public:
		Transform(
			const Vector3& Position = Vector3(0.0f),
			const Quaternion& Rotation = Quaternion(1.0, 0.0, 0.0, 0.0),
			const Vector3& Scale = Vector3(1.0f)
		);

		~Transform() = default;

		inline const Mat4x4& GetTransformationMatrix() const { return m_TransformationMatrix; }
		inline void SetTransformationMatrix(const Mat4x4& transformationMatrix) { m_TransformationMatrix = transformationMatrix; }

		inline const Vector3& GetPosition() const { return m_Position; }
		inline const Quaternion& GetRotation() const { return m_Rotation; }
		inline const Vector3& GetScale() const { return m_Scale; }

		inline void SetPosition(const Vector3& position) { m_Position = position; RecalculateTransformationMatrix(); }
		inline void SetRotation(const Quaternion& rotation) { m_Rotation = rotation; RecalculateTransformationMatrix(); }
		inline void SetScale(const Vector3& scale) { m_Scale = scale; RecalculateTransformationMatrix(); }

		operator Mat4x4& () { return m_TransformationMatrix; }
		operator const Mat4x4& () const { return m_TransformationMatrix; }
	private:
		void RecalculateTransformationMatrix();
	private:
		Mat4x4 m_TransformationMatrix;

		Vector3 m_Position;
		Quaternion m_Rotation;
		Vector3 m_Scale;
	};
}