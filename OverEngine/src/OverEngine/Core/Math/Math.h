#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Color.h"

namespace OverEngine
{
	namespace Math
	{
		using Vector1 = glm::vec1;
		using Vector2 = glm::vec2;
		using Vector3 = glm::vec3;
		using Vector4 = glm::vec4;

		using Mat3x3 = glm::mat3;
		using Mat4x4 = glm::mat4;

		using Quaternion = glm::quat;

		inline Quaternion QuaternionEuler(Vector3 rot)
		{
			return
				glm::angleAxis(glm::radians(rot.x), Vector3{ 1, 0, 0 }) *
				glm::angleAxis(glm::radians(rot.y), Vector3{ 0, 1, 0 }) *
				glm::angleAxis(glm::radians(rot.z), Vector3{ 0, 0, 1 });
		}

		inline Vector3 QuaternionEulerAngles(Quaternion rot) { return glm::eulerAngles(rot); }

		template<typename T>
		T Clamp(T val, T min, T max)
		{
			if (val > max)
				return max;
			if (val < min)
				return min;
			return val;
		}
	}
}