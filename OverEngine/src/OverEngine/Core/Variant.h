#pragma once

#include "OverEngine/Core/Core.h"

#include <variant>

namespace OverEngine
{
	class Object;
	struct TypeInfo;

	class Variant
	{
	public:
		inline Variant() { Clear(); }
		inline Variant(const Variant& value) { operator=(value); }

		inline Variant(const int8_t& value) { operator=(value); }
		inline Variant(const int16_t& value) { operator=(value); }
		inline Variant(const int32_t& value) { operator=(value); }
		inline Variant(const int64_t& value) { operator=(value); }

		inline Variant(const uint8_t& value) { operator=(value); }
		inline Variant(const uint16_t& value) { operator=(value); }
		inline Variant(const uint32_t& value) { operator=(value); }
		inline Variant(const uint64_t& value) { operator=(value); }

		inline Variant(const bool& value) { operator=(value); }

		inline Variant(const float& value) { operator=(value); }
		inline Variant(const double& value) { operator=(value); }

		inline Variant(const Vector2& value) { operator=(value); }
		inline Variant(const Vector3& value) { operator=(value); }
		inline Variant(const Vector4& value) { operator=(value); }

		inline Variant(const String& value) { operator=(value); }

		template<typename T>
		inline Variant(const Ref<T>& value) { operator=(value); }

		inline void Clear() { m_Value = (int64_t)0; m_Type = nullptr; }
		inline Variant& operator=(const Variant& value) { m_Value = value.m_Value; m_Type = value.m_Type; return *this; }

		Variant& operator=(const int8_t& value);
		Variant& operator=(const int16_t& value);
		Variant& operator=(const int32_t& value);
		Variant& operator=(const int64_t& value);

		Variant& operator=(const uint8_t& value);
		Variant& operator=(const uint16_t& value);
		Variant& operator=(const uint32_t& value);
		Variant& operator=(const uint64_t& value);

		Variant& operator=(const bool& value);

		Variant& operator=(const float& value);
		Variant& operator=(const double& value);

		Variant& operator=(const Vector2& value);
		Variant& operator=(const Vector3& value);
		Variant& operator=(const Vector4& value);

		Variant& operator=(const String& value);

		template<typename T>
		Variant& operator=(const Ref<T>& value)
		{
			if (value)
			{
				m_Value = value;
				m_Type = (TypeInfo*)value->DynamicReflect();
			}
			else
			{
				Clear();
			}

			return *this;
		}

		inline operator int8_t() const { return (int8_t)std::get<int64_t>(m_Value); }
		inline operator int16_t() const { return (int16_t)std::get<int64_t>(m_Value); }
		inline operator int32_t() const { return (int32_t)std::get<int64_t>(m_Value); }
		inline operator int64_t() const { return std::get<int64_t>(m_Value); }

		inline operator uint8_t() const { return (int8_t)std::get<int64_t>(m_Value); }
		inline operator uint16_t() const { return (int16_t)std::get<int64_t>(m_Value); }
		inline operator uint32_t() const { return (int32_t)std::get<int64_t>(m_Value); }
		inline operator uint64_t() const { return (int64_t)std::get<int64_t>(m_Value); }

		inline operator bool() const { return (bool)std::get<int64_t>(m_Value); }

		inline operator float() const { return (float)std::get<double>(m_Value); }
		inline operator double() const { return std::get<double>(m_Value); }

		inline operator Vector2() const { return std::get<Vector2>(m_Value); }
		inline operator Vector3() const { return std::get<Vector3>(m_Value); }
		inline operator Vector4() const { return std::get<Vector4>(m_Value); }

		inline operator const String&() const { return std::get<String>(m_Value); }

		template<typename T>
		inline operator Ref<T>() const { return m_Type ? std::dynamic_pointer_cast<T>(std::get<Ref<Object>>(m_Value)) : nullptr; }

		inline TypeInfo* GetType() const { return m_Type; }

	private:
		TypeInfo* m_Type = nullptr;
		std::variant<int64_t, double, Vector2, Vector3, Vector4, String, Ref<Object>> m_Value;
	};

	using PropertyGetter = Variant(*)(Object*);
	using PropertySetter = void(*)(Object*, const Variant&);

	using SelfContainedPropertyGetter = std::function<Variant()>;
	using SelfContainedPropertySetter = std::function<void(const Variant&)>;
}
