#include "pcheader.h"
#include "SceneCamera.h"

namespace OverEngine
{
	OE_REFLECT_ENUM_CLASS_BEGIN(SceneCamera::ProjectionType)
	OE_REFLECT_ENUM_CLASS_VALUE(None, 0)
	OE_REFLECT_ENUM_CLASS_VALUE(Orthographic, 1)
	OE_REFLECT_ENUM_CLASS_VALUE(Perspective, 2)
	OE_REFLECT_ENUM_CLASS_END()

	OE_REFLECT_STRUCT_BEGIN(SceneCamera)
	OE_REFLECT_STRUCT_MEMBER(m_ProjectionType)

	OE_REFLECT_STRUCT_MEMBER(m_PerspectiveFOV)
	OE_REFLECT_STRUCT_MEMBER(m_PerspectiveNear)
	OE_REFLECT_STRUCT_MEMBER(m_PerspectiveFar)

	OE_REFLECT_STRUCT_MEMBER(m_OrthographicSize)
	OE_REFLECT_STRUCT_MEMBER(m_OrthographicNear)
	OE_REFLECT_STRUCT_MEMBER(m_OrthographicFar)

	OE_REFLECT_STRUCT_MEMBER(m_AspectRatio)

	OE_REFLECT_STRUCT_MEMBER(m_ClearFlags)
	OE_REFLECT_STRUCT_MEMBER(m_ClearColor)
	OE_REFLECT_STRUCT_END()

	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
		RecalculateProjection();
	}

	void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFOV = verticalFOV;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;
		RecalculateProjection();
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection()
	{
		if (m_ProjectionType == ProjectionType::Perspective)
		{
			m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		}
		else
		{
			float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
			float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
			float orthoBottom = -m_OrthographicSize * 0.5f;
			float orthoTop = m_OrthographicSize * 0.5f;

			m_Projection = glm::ortho(orthoLeft, orthoRight,
				orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
		}
	}
}
