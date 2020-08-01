#include "pcheader.h"
#include "Camera.h"

namespace OverEngine
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Camera /////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*Camera::Camera(const Math::Mat4x4& viewMat, const Math::Mat4x4& projMat, CameraType type,
				   float aspectRatio, float orthoSizeOrFov)
		: m_ViewMatrix(viewMat), m_ProjectionMatrix(projMat),
		  m_ViewProjectionMatrix(m_ProjectionMatrix * m_ViewMatrix),
		  m_Position(Math::Vector3(0.0f)), m_Rotation(Math::Vector3(0.0f)), m_Type(type),
		  m_AspectRatio(aspectRatio), m_FieldOfView(orthoSizeOrFov)
	{
	}*/

	Camera::Camera(CameraType type, float orthoSizeOrFov, float aspectRatio, float zNear, float zFar)
	{
		if (type == CameraType::Orthographic)
			MakeOrthographic(orthoSizeOrFov, aspectRatio, zNear, zFar);
		else
			MakePrespective(orthoSizeOrFov, aspectRatio, zNear, zFar);
	}

	void Camera::MakeOrthographic(float size, float aspectRatio, float zNear /*= -1.0f*/, float zFar /*= 1.0f*/)
	{
		m_ViewMatrix = Mat4x4(1.0f);
		m_ProjectionMatrix = glm::ortho(-size * aspectRatio, size * aspectRatio, -size, size, zNear, zFar);
		RecalculateViewProjectionMatrix();
		
		m_Position = Vector3(0.0f);
		m_Rotation = Vector3(0.0f);

		m_Type = CameraType::Orthographic;
		m_AspectRatio = aspectRatio;
		m_ZNear = zNear;
		m_ZFar = zFar;
		m_OrthographicSize = size;
	}

	void Camera::MakePrespective(float fov, float aspectRatio, float zNear /*= 0.05f*/, float zFar /*= 100.0f*/)
	{
		m_ViewMatrix = Mat4x4(1.0f);
		m_ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar);
		RecalculateViewProjectionMatrix();
		
		m_Position = Vector3(0.0f);
		m_Rotation = Vector3(0.0f);

		m_Type = CameraType::Prespective;
		m_AspectRatio = aspectRatio;
		m_ZNear = zNear;
		m_ZFar = zFar;
		m_FieldOfView = fov;
	}

	void Camera::RecalculateViewProjectionMatrix()
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera::RecalculateViewMatrix(bool recalculateViewProj)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
		glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(1, 0, 0)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.y), glm::vec3(0, 1, 0)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));
		m_ViewMatrix = glm::inverse(transform);

		if (recalculateViewProj)
			RecalculateViewProjectionMatrix();
	}

	void Camera::RecalculateProjectionMatrix(bool recalculateViewProj)
	{
		if (m_Type == CameraType::Orthographic)
		{
			m_ProjectionMatrix = glm::ortho(
				-m_OrthographicSize * m_AspectRatio,
				 m_OrthographicSize * m_AspectRatio,
				-m_OrthographicSize, m_OrthographicSize,
				 m_ZNear, m_ZFar
			);
		}
		else
		{
			m_ProjectionMatrix = glm::perspective(glm::radians(m_FieldOfView), m_AspectRatio, m_ZNear, m_ZFar);
		}

		if (recalculateViewProj)
			RecalculateViewProjectionMatrix();
	}
}