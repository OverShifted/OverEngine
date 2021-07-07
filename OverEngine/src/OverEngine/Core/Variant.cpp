#include "pcheader.h"
#include "Variant.h"

#include "OverEngine/Core/Runtime/Reflection/TypeInfo.h"

namespace OverEngine
{
	Variant& Variant::operator=(const int8_t& value) { m_Value = (int64_t)value; m_Type = TypeResolver<int8_t>::Get(); return *this; }
	Variant& Variant::operator=(const int16_t& value) { m_Value = (int64_t)value; m_Type = TypeResolver<int16_t>::Get(); return *this; }
	Variant& Variant::operator=(const int32_t& value) { m_Value = (int64_t)value; m_Type = TypeResolver<int32_t>::Get(); return *this; }
	Variant& Variant::operator=(const int64_t& value) { m_Value = value; m_Type = TypeResolver<int64_t>::Get(); return *this; }

	Variant& Variant::operator=(const uint8_t& value) { m_Value = (int64_t)value; m_Type = TypeResolver<uint8_t>::Get(); return *this; }
	Variant& Variant::operator=(const uint16_t& value) { m_Value = (int64_t)value; m_Type = TypeResolver<uint16_t>::Get(); return *this; }
	Variant& Variant::operator=(const uint32_t& value) { m_Value = (int64_t)value; m_Type = TypeResolver<uint32_t>::Get(); return *this; }
	Variant& Variant::operator=(const uint64_t& value) { m_Value = (int64_t)value; m_Type = TypeResolver<uint64_t>::Get(); return *this; }

	Variant& Variant::operator=(const bool& value) { m_Value = (int64_t)value; m_Type = TypeResolver<bool>::Get(); return *this; }

	Variant& Variant::operator=(const float& value) { m_Value = (double)value; m_Type = TypeResolver<float>::Get(); return *this; }
	Variant& Variant::operator=(const double& value) { m_Value = value; m_Type = TypeResolver<double>::Get(); return *this; }

	Variant& Variant::operator=(const Vector2& value) { m_Value = value; m_Type = TypeResolver<Vector2>::Get(); return *this; }
	Variant& Variant::operator=(const Vector3& value) { m_Value = value; m_Type = TypeResolver<Vector3>::Get(); return *this; }
	Variant& Variant::operator=(const Vector4& value) { m_Value = value; m_Type = TypeResolver<Vector4>::Get(); return *this; }

	Variant& Variant::operator=(const String& value) { m_Value = value; m_Type = TypeResolver<String>::Get(); return *this; }
}
