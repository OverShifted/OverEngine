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
		// Camera(const Math::Mat4x4& viewMat, const Math::Mat4x4& projMat, CameraType type, float aspectRatio, float orthoSizeOrFov);
		Camera(CameraType type, float orthoSizeOrFov, float aspectRatio, float zNear, float zFar);
		Camera() = default;

		~Camera() = default;

		void MakeOrthographic(float size, float aspectRatio, float zNear = -1.0f, float zFar = 1.0f);
		void MakePrespective(float fov, float aspectRatio, float zNear = 0.05f, float zFar = 100.0f);

		inline const Math::Mat4x4& GetViewMatrix() const { return m_ViewMatrix; }
		inline const Math::Mat4x4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		inline const Math::Mat4x4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		inline void SetViewMatrix(const Math::Mat4x4& viewMatrix) { m_ViewMatrix = viewMatrix; RecalculateViewMatrix(); }
		inline void SetProjectionMatrix(const Math::Mat4x4& projMatrix) { m_ProjectionMatrix = projMatrix; RecalculateProjectionMatrix(); }

		inline CameraType GetType() { return m_Type; }
		inline bool IsOrthographic() const { return m_Type == CameraType::Orthographic; }
		inline bool IsPrespective() const { return m_Type == CameraType::Prespective; }

		inline const Math::Vector3& GetPosition() const { return m_Position; }
		inline const Math::Vector3& GetRotation() const { return m_Rotation; }

		inline void SetPosition(const Math::Vector3& position) { m_Position = position; RecalculateViewMatrix(); }
		inline void SetRotation(const Math::Vector3& rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

		inline void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; RecalculateProjectionMatrix(); }
		inline float GetAspectRatio() const { return m_AspectRatio; }

		inline void SetOrthographicSize(float orthographicSize) { m_OrthographicSize = orthographicSize; RecalculateProjectionMatrix(); }
		inline float GetOrthographicSize() const { return m_OrthographicSize; }

		inline void SetFieldOfView(float fov) { m_FieldOfView = fov; RecalculateProjectionMatrix(); }
		inline float GetFieldOfView() const { return m_FieldOfView; }

		inline void SetZNear(float value) { m_ZNear = value; RecalculateProjectionMatrix(); }
		inline float GetZNear() const { return m_ZNear; }

		inline void SetZFar(float value) { m_ZFar = value; RecalculateProjectionMatrix(); }
		inline float GetZFar() const { return m_ZFar; }
	protected:
		void RecalculateViewMatrix(bool recalculateViewProj = true);
		void RecalculateProjectionMatrix(bool recalculateViewProj = true);
		void RecalculateViewProjectionMatrix();
	protected:
		Math::Mat4x4 m_ViewMatrix;
		Math::Mat4x4 m_ProjectionMatrix;
		Math::Mat4x4 m_ViewProjectionMatrix;

		Math::Vector3 m_Position;
		Math::Vector3 m_Rotation;

		CameraType m_Type;
		float m_AspectRatio;
		float m_ZNear, m_ZFar;
		union { float m_FieldOfView, m_OrthographicSize; };
	};
}