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
		// Camera(const Mat4x4& viewMat, const Mat4x4& projMat, CameraType type, float aspectRatio, float orthoSizeOrFov);
		Camera(CameraType type, float orthoSizeOrFov, float aspectRatio, float zNear, float zFar);
		Camera() = default;

		~Camera() = default;

		void MakeOrthographic(float size, float aspectRatio, float zNear = -1.0f, float zFar = 1.0f);
		void MakePrespective(float fov, float aspectRatio, float zNear = 0.05f, float zFar = 100.0f);

		inline const Mat4x4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

		inline void SetProjectionMatrix(const Mat4x4& projMatrix) { m_ProjectionMatrix = projMatrix; RecalculateProjectionMatrix(); }

		inline CameraType GetType() { return m_Type; }
		inline void SetType(CameraType type) { m_Type = type; RecalculateProjectionMatrix(); }

		inline bool IsOrthographic() const { return m_Type == CameraType::Orthographic; }
		inline bool IsPrespective() const { return m_Type == CameraType::Prespective; }

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

		inline void SetClearColor(const Color& color) { m_ClearColor = color; }
		inline const Color& GetClearColor() { return m_ClearColor; }

		inline void SetIsClearingColor(bool value) { m_IsClearingColor = value; }
		inline bool GetIsClearingColor() { return m_IsClearingColor; }

		inline void SetIsClearingDepth(bool value) { m_IsClearingDepth = value; }
		inline bool GetIsClearingDepth() { return m_IsClearingDepth; }
	protected:
		void RecalculateProjectionMatrix();
	protected:
		// Projection Stuff
		CameraType m_Type;
		float m_AspectRatio;
		float m_ZNear, m_ZFar;
		union { float m_FieldOfView, m_OrthographicSize; };

		// Runtime
		Mat4x4 m_ProjectionMatrix;

		bool m_IsClearingColor = true;
		bool m_IsClearingDepth = true;
		Color m_ClearColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
	};
}