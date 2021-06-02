#pragma once

#include "OverEngine/Renderer/Camera.h"

namespace OverEngine
{
	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType : uint8_t
		{
			None = 0,
			Orthographic,
			Perspective
		};

	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthographic(float size, float nearClip, float farClip);
		void SetPerspective(float verticalFOV, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		float GetPerspectiveFOV() const { return m_PerspectiveFOV; }
		void SetPerspectiveFOV(float fov) { m_PerspectiveFOV = fov; RecalculateProjection(); }
		float GetPerspectiveNearClip() const { return m_PerspectiveNearClip; }
		void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNearClip = nearClip; RecalculateProjection(); }
		float GetPerspectiveFarClip() const { return m_PerspectiveFarClip; }
		void SetPerspectiveFarClip(float farClip) { m_PerspectiveFarClip = farClip; RecalculateProjection(); }

		float GetOrthographicSize() const { return m_OrthographicSize; }
		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
		float GetOrthographicNearClip() const { return m_OrthographicNearClip; }
		void SetOrthographicNearClip(float nearClip) { m_OrthographicNearClip = nearClip; RecalculateProjection(); }
		float GetOrthographicFarClip() const { return m_OrthographicFarClip; }
		void SetOrthographicFarClip(float farClip) { m_OrthographicFarClip = farClip; RecalculateProjection(); }

		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; RecalculateProjection(); }

		inline void SetClearColor(const Color& color) { m_ClearColor = color; }
		inline const Color& GetClearColor() const { return m_ClearColor; }

		inline bool IsClearingColor() const { return m_ClearFlags & ClearFlags_ClearColor; }
		inline bool IsClearingDepth() const { return m_ClearFlags & ClearFlags_ClearDepth; }

		inline ClearFlags& GetClearFlags() { return m_ClearFlags; }
		inline ClearFlags GetClearFlags() const { return m_ClearFlags; }
		inline void SetClearFlags(ClearFlags flags) { m_ClearFlags = flags; }

	protected:
		void RecalculateProjection();

	protected:
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNearClip = 0.01f, m_PerspectiveFarClip = 1000.0f;

		float m_OrthographicSize = 10.0f;
		float m_OrthographicNearClip = -1.0f, m_OrthographicFarClip = 1.0f;

		float m_AspectRatio = 0.0f;

		ClearFlags m_ClearFlags = ClearFlags_ClearColor | ClearFlags_ClearDepth;
		Color m_ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	};
}
