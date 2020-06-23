#include "pcheader.h"

#include "Transform.h"

namespace OverEngine
{
	Transform::Transform(
		const Math::Vector3& Position,
		const Math::Quaternion& Rotation,
		const Math::Vector3& Scale /*= Math::Vector3(1.0f) */)
		: m_Position(Position), m_Rotation(Rotation), m_Scale(Scale)
	{
		RecalculateTransformationMatrix();
	}

	void Transform::RecalculateTransformationMatrix()
	{
		m_TransformationMatrix =
			glm::translate(glm::mat4(1.0), m_Position) *
			glm::mat4_cast(m_Rotation) *
			glm::scale(glm::mat4(1.0), m_Scale);
	}
}