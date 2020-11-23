#include "pcheader.h"
#include "SceneCamera.h"

namespace OverEngine
{
	SerializationContext* SceneCamera::Reflect()
	{
		static bool initialized = false;
		static SerializationContext ctx;

		if (!initialized)
		{
			initialized = true;

			if (!Serializer::GlobalEnumExists("SceneCamera::ProjectionType"))
			{
				Serializer::DefineGlobalEnum("SceneCamera::ProjectionType", {
					{ 0, "Orthographic" },
					{ 1, "Perspective" }
				});
			}

			ctx.AddEnumField(SerializableType::IntEnum, "SceneCamera::ProjectionType", &SceneCamera::m_ProjectionType, "ProjectionType");

			ctx.AddField(SerializableType::Float, &SceneCamera::m_PerspectiveFOV, "PerspectiveFOV");
			ctx.AddField(SerializableType::Float, &SceneCamera::m_PerspectiveNear, "PerspectiveNear");
			ctx.AddField(SerializableType::Float, &SceneCamera::m_PerspectiveFar, "PerspectiveFar");

			ctx.AddField(SerializableType::Float, &SceneCamera::m_OrthographicSize, "OrthographicSize");
			ctx.AddField(SerializableType::Float, &SceneCamera::m_OrthographicNear, "OrthographicNear");
			ctx.AddField(SerializableType::Float, &SceneCamera::m_OrthographicFar, "OrthographicFar");

			ctx.AddField(SerializableType::Int8, &SceneCamera::m_ClearFlags, "ClearFlags");
			ctx.AddField(SerializableType::Float4, &SceneCamera::m_ClearColor, "ClearColor");
		}

		return &ctx;
	}

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
