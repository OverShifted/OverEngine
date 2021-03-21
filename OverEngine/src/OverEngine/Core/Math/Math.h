#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/fast_square_root.hpp>

#include "Color.h"

namespace OverEngine
{
	namespace Math
	{
		using Vector1 = glm::vec1;
		using Vector2 = glm::vec2;
		using Vector3 = glm::vec3;
		using Vector4 = glm::vec4;

		using Rect = Vector4;

		using Mat3x3 = glm::mat3;
		using Mat4x4 = glm::mat4;
		#define IDENTITY_MAT3X3 ::OverEngine::Math::Mat3x3(1.0f)
		#define IDENTITY_MAT4X4 ::OverEngine::Math::Mat4x4(1.0f)

		using Quaternion = glm::quat;
		#define IDENTITY_QUATERNION ::OverEngine::Math::Quaternion(1.0, 0.0, 0.0, 0.0)

		#define SCALE_MAT4X4(_s) Mat4x4(_s.x, 0.0f, 0.0f, 0.0f, \
										0.0f, _s.y, 0.0f, 0.0f, \
										0.0f, 0.0f, _s.z, 0.0f, \
										0.0f, 0.0f, 0.0f, 1.0f)

		enum class Axis : uint8_t
		{
			None, X, Y, Z
		};

		inline Quaternion EulerAnglesToQuaternion(Vector3 rot)
		{
			return
				glm::angleAxis(glm::radians(rot.x), Vector3{ 1, 0, 0 }) *
				glm::angleAxis(glm::radians(rot.y), Vector3{ 0, 1, 0 }) *
				glm::angleAxis(glm::radians(rot.z), Vector3{ 0, 0, 1 });
		}

		// Returns EulerAngles in degrees
		inline Vector3 QuaternionToEulerAngles(Quaternion rot)
		{
			return { glm::degrees(pitch(rot)), glm::degrees(yaw(rot)), glm::degrees(roll(rot)) };
		}

		// Returns EulerAngles in radians
		inline Vector3 QuaternionToEulerAnglesRadians(Quaternion rot) { return glm::eulerAngles(rot); }

		template<typename T>
		T Clamp(T val, T min, T max)
		{
			if (val > max)
				return max;
			else if (val < min)
				return min;
			return val;
		}
	}

	using namespace Math;
}
