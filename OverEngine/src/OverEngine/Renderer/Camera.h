#pragma once

#include "OverEngine/Core/Math/Math.h"

namespace OverEngine
{
	enum class CameraType
	{
		Orthographic, Prespective
	};

	class Camera
	{
	public:
		Camera(const Math::Mat4x4& viewMat, const Math::Mat4x4& projMat, CameraType type, float aspectRatio, float orthoSizeOrFov);
		~Camera() = default;

	public:
		inline const Math::Mat4x4& GetViewMatrix() const { return m_ViewMatrix; }
		inline const Math::Mat4x4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		inline const Math::Mat4x4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		inline void SetViewMatrix(const Math::Mat4x4& viewMatrix) { m_ViewMatrix = viewMatrix; RecalculateViewMatrix(); }
		inline void SetProjectionMatrix(const Math::Mat4x4& projMatrix) { m_ProjectionMatrix = projMatrix; RecalculateProjectionMatrix(); }

		inline const CameraType GetType() { return m_Type; }
		inline const bool IsOrthographic() const { return m_Type == CameraType::Orthographic; }
		inline const bool IsPrespective() const { return m_Type == CameraType::Prespective; }

		inline const Math::Vector3& GetPosition() const { return m_Position; }
		inline const Math::Vector3& GetRotation() const { return m_Rotation; }

		inline void SetPosition(const Math::Vector3& position) { m_Position = position; RecalculateViewMatrix(); }
		inline void SetRotation(const Math::Vector3& rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

		inline void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; RecalculateProjectionMatrix(); }
		inline const float& GetAspectRatio() const { return m_AspectRatio; }

		inline void SetOrthographicSize(float orthographicSize) { m_OrthographicSize = orthographicSize; RecalculateProjectionMatrix(); }
		inline const float& GetOrthographicSize() const { return m_OrthographicSize; }

		inline void SetFieldOfView(float fov) { m_FieldOfView = fov; RecalculateProjectionMatrix(); }
		inline const float& GetFieldOfView() const { return m_FieldOfView; }
	protected:
		void RecalculateViewProjectionMatrix();
		void RecalculateViewMatrix();
		virtual void RecalculateProjectionMatrix() = 0;
	protected:
		Math::Mat4x4 m_ViewMatrix;
		Math::Mat4x4 m_ProjectionMatrix;
		Math::Mat4x4 m_ViewProjectionMatrix;

		Math::Vector3 m_Position;
		Math::Vector3 m_Rotation;

		CameraType m_Type;
		float m_AspectRatio;
		union { float m_FieldOfView, m_OrthographicSize; };
	};

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float size = 1.0f, float aspectRatio = 1.0f, float zNear = -1.0f, float zFar = 1.0f);
		~OrthographicCamera() = default;
	protected:
		virtual void RecalculateProjectionMatrix() override;
	};

	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(float fov = 60.0f, float aspectRatio = 1.0f, float zNear = 0.05f, float zFar = 100.0f);
		~PerspectiveCamera() = default;
	protected:
		virtual void RecalculateProjectionMatrix() override;
	};
}