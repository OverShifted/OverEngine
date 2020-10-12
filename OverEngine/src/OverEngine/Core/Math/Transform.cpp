#include "pcheader.h"
#include "Transform.h"

namespace OverEngine
{
	Transform::Transform(const Vector3& position, const Quaternion& rotation, const Vector3& scale)
		: m_Rotation(rotation), m_Scale(scale)
	{
		m_EulerAngles = QuaternionToEulerAngles(m_Rotation);
		m_RotationMatrix = glm::mat4_cast(m_Rotation);
		RecalculateMatrix(position);
	}

	const Mat4x4& Transform::GetMatrix()
	{
		// Recalculate matrix just if transform is changed and we wanna get the matrix
		if (m_ChangedFlags & RecalculateNeeded)
		{
			if (m_ChangedFlags & RotationChanged)
			{
				m_RotationMatrix = glm::mat3_cast(m_Rotation);
				m_ChangedFlags ^= RotationChanged;
			}

			RecalculateMatrix();
			m_ChangedFlags ^= RecalculateNeeded;
		}

		return m_Matrix;
	}

	Mat4x4 Transform::GetMatrix() const
	{
		// We cant write to m_RotationMatrix or m_Matrix
		// so lets write to a new one

		if (m_ChangedFlags & RecalculateNeeded)
		{
			OE_WARN("Performance warning: recalculating matrix in temporary space because this Transform instance in const");

			Vector3 lastPos = GetPosition();

			Mat4x4 matrix;
			if (m_ChangedFlags & RotationChanged)
			{
				matrix = glm::mat4_cast(m_Rotation) *
					Mat4x4(m_Scale.x, 0.0f, 0.0f, 0.0f,
						   0.0f, m_Scale.y, 0.0f, 0.0f,
						   0.0f, 0.0f, m_Scale.z, 0.0f,
						   0.0f, 0.0f, 0.0f, 1.0f);
			}

			matrix = Mat4x4(m_RotationMatrix) *
				Mat4x4(m_Scale.x, 0.0f, 0.0f, 0.0f,
					   0.0f, m_Scale.y, 0.0f, 0.0f,
					   0.0f, 0.0f, m_Scale.z, 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f);

			matrix[3].x = lastPos.x;
			matrix[3].y = lastPos.y;
			matrix[3].z = lastPos.z;

			return matrix;
		}

		return m_Matrix;
	}

	void Transform::SetPosition(const Vector3& position)
	{
		m_Matrix[3].x = position.x;
		m_Matrix[3].y = position.y;
		m_Matrix[3].z = position.z;

		m_ChangedFlags |= Changed;
	}

	void Transform::SetEulerAngles(const Vector3& rotation)
	{
		m_EulerAngles = rotation;
		m_Rotation = EulerAnglesToQuaternion(m_EulerAngles);

		m_ChangedFlags |= RotationChanged;
		m_ChangedFlags |= RecalculateNeeded;
	}

	void Transform::SetRotation(const Quaternion& rotation)
	{
		m_Rotation = rotation;
		m_EulerAngles = QuaternionToEulerAngles(m_Rotation);

		m_ChangedFlags |= RotationChanged;
		m_ChangedFlags |= RecalculateNeeded;
	}

	void Transform::SetScale(const Vector3& scale)
	{
		m_Scale = scale;
		m_ChangedFlags |= RecalculateNeeded;
	}

	void Transform::RecalculateMatrix(const Vector3& position)
	{
		m_Matrix = Mat4x4(m_RotationMatrix) * SCALE_MAT4X4(m_Scale);
		SetPosition(position);
	}
}
