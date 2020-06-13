#include "pcheader.h"
#include "Camera.h"

namespace OverEngine
{
	namespace Renderer
	{

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// Camera /////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		Camera::Camera(Math::Mat4x4 viewMat, Math::Mat4x4 projMat)
			: m_ViewMat(viewMat), m_ProjectionMat(projMat), m_ViewProjectionMat(m_ProjectionMat * m_ViewMat)
		{
		}

		void Camera::RecalculateViewProjectionMatrix()
		{
			m_ViewProjectionMat = m_ProjectionMat * m_ViewMat;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// OrthographicCamera /////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		OrthographicCamera::OrthographicCamera(float size, float aspectRatio)
			: Camera(Math::Mat4x4(1.0f), glm::ortho(-size * aspectRatio, size* aspectRatio, -size, size)),
			  m_AspectRatio(aspectRatio), m_Size(size)
		{
		}


		void OrthographicCamera::SetOrthographicSize(float size)
		{
			m_Size = size;
			RecalculateProjectionMatrix();
		}

		void OrthographicCamera::SetAspectRatio(float aspectRatio)
		{
			m_AspectRatio = aspectRatio;
			RecalculateProjectionMatrix();
		}

		void OrthographicCamera::RecalculateProjectionMatrix()
		{
			m_ProjectionMat = glm::ortho(-m_Size * m_AspectRatio, m_Size * m_AspectRatio, -m_Size, m_Size);
			RecalculateViewProjectionMatrix();
		}

	}
}