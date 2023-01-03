#include "pcheader.h"
#include "Transform.h"

namespace OverEngine
{
	Transform::Transform(const Vector3& position, const Quaternion& rotation, const Vector3& scale)
		: m_Rotation(rotation), m_Scale(scale)
	{
		m_EulerAngles = QuaternionToEulerAngles(m_Rotation);
		RecalculateMatrix(position);
	}

	const Mat4x4& Transform::GetMatrix() const
	{
		// Recalculate matrix just if transform is changed and we wanna get the matrix
		if (m_Dirty)
		{
			RecalculateMatrix();
			m_Dirty = false;
		}

		return m_Matrix;
	}

	void Transform::SetPosition(const Vector3& position)
	{
		m_Matrix[3].x = position.x;
		m_Matrix[3].y = position.y;
		m_Matrix[3].z = position.z;
	}

	void Transform::SetEulerAngles(const Vector3& rotation)
	{
		m_EulerAngles = rotation;
		m_Rotation = EulerAnglesToQuaternion(m_EulerAngles);

		m_Dirty = true;
	}

	void Transform::SetRotation(const Quaternion& rotation)
	{
		m_Rotation = rotation;
		m_EulerAngles = QuaternionToEulerAngles(m_Rotation);

		m_Dirty = true;
	}

	void Transform::SetScale(const Vector3& scale)
	{
		m_Scale = scale;
		m_Dirty = true;
	}

	void Transform::RecalculateMatrix(const Vector3& position) const
	{
		m_Matrix = Mat4x4(glm::mat3_cast(m_Rotation)) * SCALE_MAT4X4(m_Scale);
		m_Matrix[3].x = position.x;
		m_Matrix[3].y = position.y;
		m_Matrix[3].z = position.z;
	}
}
