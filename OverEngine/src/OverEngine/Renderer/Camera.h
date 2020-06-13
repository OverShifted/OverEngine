#pragma once

namespace OverEngine
{
	namespace Renderer
	{
		class Camera
		{
		public:
			Camera(Math::Mat4x4 viewMat, Math::Mat4x4 projMat);
			~Camera() = default;

			inline const Math::Mat4x4& GetViewMatrix() const { return m_ViewMat; }
			inline const Math::Mat4x4& GetProjectionMatrix() const { return m_ProjectionMat; }
			inline const Math::Mat4x4& GetViewProjectionMatrix() const { return m_ViewProjectionMat; }

			inline void SetViewMatrix(const Math::Mat4x4 viewMatrix) { m_ViewMat = viewMatrix; }
			inline void SetProjectionMatrix(const Math::Mat4x4 projMatrix) { m_ProjectionMat = projMatrix; }

		protected:
			void RecalculateViewProjectionMatrix();
		protected:
			Math::Mat4x4 m_ViewMat;
			Math::Mat4x4 m_ProjectionMat;
			Math::Mat4x4 m_ViewProjectionMat;
		};

		class OrthographicCamera : public Camera
		{
		public:
			OrthographicCamera(float size, float aspectRatio);
			~OrthographicCamera() = default;

			void SetOrthographicSize(float size);
			void SetAspectRatio(float aspectRatio);

			inline float GetOrthographicSize() { return m_Size; }
			inline float GetAspectRatio() { return m_AspectRatio; }
		private:
			void RecalculateProjectionMatrix();
		private:
			float m_Size;
			float m_AspectRatio;
		};
	}
}