#include "pcheader.h"

#include "Transform.h"

namespace OverEngine
{
	Transform::Transform(
		const Vector3& Position /*= Vector3(0.0f)*/,
		const Quaternion& Rotation /*= Quaternion(1.0, 0.0, 0.0, 0.0)*/,
		const Vector3& Scale /*= Vector3(1.0f) */)
		: m_Position(Position), m_Rotation(Rotation), m_Scale(Scale)
	{
		RecalculateTransformationMatrix();
	}

	void Transform::RecalculateTransformationMatrix()
	{
		m_TransformationMatrix =
			glm::translate(Mat4x4(1.0), m_Position) *
			glm::mat4_cast(m_Rotation) *
			glm::scale(Mat4x4(1.0), m_Scale);
	}
}