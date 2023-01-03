#pragma once

#include "OverEngine/Core/Math/Math.h"

namespace OverEngine
{
	/**
	 * Transform utility class
	 * Not suitable to use in scenes. Instead use TransformComponent.
	 * This class dosn't support hierarchies
	 */

	class Transform
	{
	public:
		Transform(const Vector3& position = Vector3(0.0f),
				  const Quaternion& rotation = IDENTITY_QUATERNION,
				  const Vector3& scale = Vector3(1.0f));

		~Transform() = default;

		const Mat4x4& GetMatrix() const;
		operator const Mat4x4&() const { return GetMatrix(); }

		inline Vector3 GetPosition() const { return { m_Matrix[3].x, m_Matrix[3].y, m_Matrix[3].z }; }
		void SetPosition(const Vector3& position);

		inline const Vector3& GetEulerAngles() const { return m_EulerAngles; }
		void SetEulerAngles(const Vector3& rotation);

		inline const Quaternion& GetRotation() const { return m_Rotation; }
		void SetRotation(const Quaternion& rotation);

		inline const Vector3& GetScale() const { return m_Scale; }
		void SetScale(const Vector3& scale);
	private:
		inline void RecalculateMatrix() const { RecalculateMatrix(GetPosition()); }
		void RecalculateMatrix(const Vector3& position) const;
	private:
		mutable Mat4x4 m_Matrix = IDENTITY_MAT4X4;
		mutable bool m_Dirty = false;

		Vector3 m_EulerAngles;
		Quaternion m_Rotation;
		Vector3 m_Scale;
	};
}
