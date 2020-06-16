#include "pcheader.h"
#include "Camera.h"

namespace OverEngine
{
	namespace Renderer
	{

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// Camera /////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		Camera::Camera(Math::Mat4x4 viewMat, Math::Mat4x4 projMat, CameraType type,
					   float aspectRatio, float orthoSizeOrFov)
			: m_ViewMatrix(viewMat), m_ProjectionMatrix(projMat),
			  m_ViewProjectionMatrix(m_ProjectionMatrix * m_ViewMatrix),
			  m_Type(type), m_Position(Math::Vector3(0.0f)), m_Rotation(Math::Vector3(0.0f)),
			  m_AspectRatio(aspectRatio), m_FieldOfView(orthoSizeOrFov), m_OrthographicSize(orthoSizeOrFov)
		{
		}

		void Camera::RecalculateViewProjectionMatrix()
		{
			m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
		}

		void Camera::RecalculateViewMatrix()
		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(1, 0, 0)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.y), glm::vec3(0, 1, 0)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));

			m_ViewMatrix = glm::inverse(transform);
			RecalculateViewProjectionMatrix();
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// OrthographicCamera /////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		OrthographicCamera::OrthographicCamera(float size, float aspectRatio, float zNear /*= -1.0f*/, float zFar /*= 1.0f*/)
			: Camera(
				Math::Mat4x4(1.0f),
			    glm::ortho(-size * aspectRatio, size* aspectRatio, -size, size, zNear, zFar),
			  	CameraType::Orthographic, aspectRatio, size
			  )
		{
		}

		void OrthographicCamera::RecalculateProjectionMatrix()
		{
			m_ProjectionMatrix = glm::ortho(-m_OrthographicSize * m_AspectRatio, m_OrthographicSize * m_AspectRatio, -m_OrthographicSize, m_OrthographicSize);
			RecalculateViewProjectionMatrix();
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// PerspectiveCamera //////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float zNear /*= 0.05f*/, float zFar /*= 100.0f*/)
			: Camera(
				Math::Mat4x4(1.0f),
				glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar),
			  	CameraType::Orthographic, aspectRatio, fov
			  )
		{
		}

		void PerspectiveCamera::RecalculateProjectionMatrix()
		{
			m_ProjectionMatrix = glm::perspective(glm::radians(m_FieldOfView), m_AspectRatio, 0.1f, 100.0f);
			RecalculateViewProjectionMatrix();
		}

	}
}