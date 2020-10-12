#pragma once

#include "OverEngine/Core/Math/Math.h"

namespace OverEngine
{
	/**
	 * Transform utility class
	 * Helps to calculate a transformation matrix for some position, rotation and scale
	 * Not suitable to use in scenes. Instead; use SceneTransform defined in OverEngine/Scene
	 * This class nose not support nesting
	 */

	class Transform
	{
	public:
		Transform(const Vector3& position = Vector3(0.0f),
				  const Quaternion& rotation = IDENTITY_QUATERNION,
				  const Vector3& scale = Vector3(1.0f));

		~Transform() = default;

		const Mat4x4& GetMatrix();
		operator const Mat4x4&() { return GetMatrix(); }

		Mat4x4 GetMatrix() const;
		operator Mat4x4() const { return GetMatrix(); }

		inline Vector3 GetPosition() const { return { m_Matrix[3].x, m_Matrix[3].y, m_Matrix[3].z }; }
		void SetPosition(const Vector3& position);

		inline const Vector3& GetEulerAngles() const { return m_EulerAngles; }
		void SetEulerAngles(const Vector3& rotation);

		inline const Quaternion& GetRotation() const { return m_Rotation; }
		void SetRotation(const Quaternion& rotation);

		inline const Vector3& GetScale() const { return m_Scale; }
		void SetScale(const Vector3& scale);

		// Returns true if transform is changed since last matrix recalculation
		inline bool IsChanged() const { return m_ChangedFlags & Changed; }
	private:
		enum ChangedFlags
		{
			None = 0, Changed = BIT(1), RecalculateNeeded = BIT(2), RotationChanged = BIT(3)
		};

		inline void RecalculateMatrix() { RecalculateMatrix(GetPosition()); }
		void RecalculateMatrix(const Vector3& position);
	private:
		Mat4x4 m_Matrix = IDENTITY_MAT4X4;
		Mat3x3 m_RotationMatrix = IDENTITY_MAT3X3;

		int8_t m_ChangedFlags = None;

		Vector3 m_EulerAngles;
		Quaternion m_Rotation;
		Vector3 m_Scale;
	};
}
