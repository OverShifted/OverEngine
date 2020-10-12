#pragma once

#include "OverEngine/Core/Math/Math.h"
#include "OverEngine/Renderer/ClearFlags.h"

namespace OverEngine
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(const Mat4x4& projection)
			: m_Projection(projection) {}

		virtual ~Camera() = default;

		const Mat4x4& GetProjection() const { return m_Projection; }
	protected:
		Mat4x4 m_Projection = IDENTITY_MAT4X4;
	};
}
