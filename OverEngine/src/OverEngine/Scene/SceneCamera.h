#pragma once

#include "OverEngine/Renderer/Camera.h"

#include "OverEngine/Core/Serialization/Serializer.h"

namespace OverEngine
{
	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType { Orthographic, Perspective };
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthographic(float size, float nearClip, float farClip);
		void SetPerspective(float verticalFOV, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
		void SetPerspectiveVerticalFOV(float verticalFov) { m_PerspectiveFOV = verticalFov; RecalculateProjection(); }
		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; RecalculateProjection(); }
		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
		void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; RecalculateProjection(); }

		float GetOrthographicSize() const { return m_OrthographicSize; }
		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
		float GetOrthographicNearClip() const { return m_OrthographicNear; }
		void SetOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); }
		float GetOrthographicFarClip() const { return m_OrthographicFar; }
		void SetOrthographicFarClip(float farClip) { m_OrthographicFar = farClip; RecalculateProjection(); }

		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; RecalculateProjection(); }

		inline void SetClearColor(const Color& color) { m_ClearColor = color; }
		inline const Color& GetClearColor() const { return m_ClearColor; }

		inline bool IsClearingColor() const { return m_ClearFlags & ClearFlags_ClearColor; }
		inline bool IsClearingDepth() const { return m_ClearFlags & ClearFlags_ClearDepth; }

		inline ClearFlags& GetClearFlags() { return m_ClearFlags; }
		inline ClearFlags GetClearFlags() const { return m_ClearFlags; }
		inline void SetClearFlags(ClearFlags flags) { m_ClearFlags = flags; }

		static SerializationContext* Reflect()
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

				ctx.AddEnumField(SerializableDataType::IntEnum, "SceneCamera::ProjectionType", SERIALIZE_FIELD(SceneCamera, m_ProjectionType));

				ctx.AddField(SerializableDataType::Float, SERIALIZE_FIELD(SceneCamera, m_PerspectiveFOV));
				ctx.AddField(SerializableDataType::Float, SERIALIZE_FIELD(SceneCamera, m_PerspectiveNear));
				ctx.AddField(SerializableDataType::Float, SERIALIZE_FIELD(SceneCamera, m_PerspectiveFar));

				ctx.AddField(SerializableDataType::Float, SERIALIZE_FIELD(SceneCamera, m_OrthographicSize));
				ctx.AddField(SerializableDataType::Float, SERIALIZE_FIELD(SceneCamera, m_OrthographicNear));
				ctx.AddField(SerializableDataType::Float, SERIALIZE_FIELD(SceneCamera, m_OrthographicFar));

				ctx.AddField(SerializableDataType::Int8, SERIALIZE_FIELD(SceneCamera, m_ClearFlags));
				ctx.AddField(SerializableDataType::Float4, SERIALIZE_FIELD(SceneCamera, m_ClearColor));
			}

			return &ctx;
		}
	protected:
		void RecalculateProjection();
	protected:
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.0f;

		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

		float m_AspectRatio = 0.0f;

		ClearFlags m_ClearFlags = ClearFlags_ClearColor | ClearFlags_ClearDepth;
		Color m_ClearColor = Color(0.0f, 0.1f, 0.5f, 1.0f);
	};
}
