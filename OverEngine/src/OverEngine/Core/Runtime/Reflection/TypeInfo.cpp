#include <OverEngine/Scene/Components.h>
#include "pcheader.h"
#include "TypeInfo.h"

#include "OverEngine/ImGui/UIElements.h"

namespace OverEngine
{
	static void DrawEnumInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const Map<int, String>& values, bool isFlag)
	{
		UIElements::BasicEnum_U(name, values, isFlag,
			[oracle, getter]() -> Variant
			{
				return getter(reinterpret_cast<Object*>(oracle()));
			},
			[oracle, setter](const Variant& value)
			{
				setter(reinterpret_cast<Object*>(oracle()), value);
			}
		);
	}

	void PrimitiveTypeInfo_int8_t::DrawInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const PropertyHint& hint)
	{
		if (hint.Type == PropertyHintType::Enum)
		{
			DrawEnumInspectorGUI(name, oracle, getter, setter, hint.EnumValues, false);
		}
		else if (hint.Type == PropertyHintType::Flags)
		{
			DrawEnumInspectorGUI(name, oracle, getter, setter, hint.EnumValues, true);
		}
	}

	void PrimitiveTypeInfo_int16_t::DrawInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const PropertyHint& hint)
	{
		if (hint.Type == PropertyHintType::Enum)
		{
			DrawEnumInspectorGUI(name, oracle, getter, setter, hint.EnumValues, false);
		}
		else if (hint.Type == PropertyHintType::Flags)
		{
			DrawEnumInspectorGUI(name, oracle, getter, setter, hint.EnumValues, true);
		}
	}

	void PrimitiveTypeInfo_int32_t::DrawInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const PropertyHint& hint)
	{
		if (hint.Type == PropertyHintType::Enum)
		{
			DrawEnumInspectorGUI(name, oracle, getter, setter, hint.EnumValues, false);
		}
		else if (hint.Type == PropertyHintType::Flags)
		{
			DrawEnumInspectorGUI(name, oracle, getter, setter, hint.EnumValues, true);
		}
	}

	void PrimitiveTypeInfo_int64_t::DrawInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const PropertyHint& hint)
	{
		if (hint.Type == PropertyHintType::Enum)
		{
			DrawEnumInspectorGUI(name, oracle, getter, setter, hint.EnumValues, false);
		}
		else if (hint.Type == PropertyHintType::Flags)
		{
			DrawEnumInspectorGUI(name, oracle, getter, setter, hint.EnumValues, true);
		}
	}

	void PrimitiveTypeInfo_uint8_t::DrawInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const PropertyHint& hint)
	{
		if (hint.Type == PropertyHintType::Enum)
		{
			DrawEnumInspectorGUI(name, oracle, getter, setter, hint.EnumValues, false);
		}
		else if (hint.Type == PropertyHintType::Flags)
		{
			DrawEnumInspectorGUI(name, oracle, getter, setter, hint.EnumValues, true);
		}
	}

	void PrimitiveTypeInfo_uint16_t::DrawInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const PropertyHint& hint)
	{
		if (hint.Type == PropertyHintType::Enum)
		{
			DrawEnumInspectorGUI(name, oracle, getter, setter, hint.EnumValues, false);
		}
		else if (hint.Type == PropertyHintType::Flags)
		{
			DrawEnumInspectorGUI(name, oracle, getter, setter, hint.EnumValues, true);
		}
	}

	void PrimitiveTypeInfo_uint32_t::DrawInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const PropertyHint& hint)
	{
		if (hint.Type == PropertyHintType::Enum)
		{
			DrawEnumInspectorGUI(name, oracle, getter, setter, hint.EnumValues, false);
		}
		else if (hint.Type == PropertyHintType::Flags)
		{
			DrawEnumInspectorGUI(name, oracle, getter, setter, hint.EnumValues, true);
		}
	}

	void PrimitiveTypeInfo_uint64_t::DrawInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const PropertyHint& hint)
	{
		if (hint.Type == PropertyHintType::Enum)
		{
			DrawEnumInspectorGUI(name, oracle, getter, setter, hint.EnumValues, false);
		}
		else if (hint.Type == PropertyHintType::Flags)
		{
			DrawEnumInspectorGUI(name, oracle, getter, setter, hint.EnumValues, true);
		}
	}

	void PrimitiveTypeInfo_bool::DrawInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const PropertyHint& hint)
	{
		UIElements::CheckboxField_U(name,
			[oracle, getter]() -> Variant
			{
				return getter(reinterpret_cast<Object*>(oracle()));
			},
			[oracle, setter](const Variant& value)
			{
				setter(reinterpret_cast<Object*>(oracle()), value);
			}
		);
	}

	void PrimitiveTypeInfo_float::DrawInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const PropertyHint& hint)
	{
		UIElements::DragFloatField_U(name,
			[oracle, getter]() -> Variant
			{
				return getter(reinterpret_cast<Object*>(oracle()));
			},
			[oracle, setter](const Variant& value)
			{
				setter(reinterpret_cast<Object*>(oracle()), value);
			}
		);
	}

	void PrimitiveTypeInfo_double::DrawInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const PropertyHint& hint)
	{
	}

	void PrimitiveTypeInfo_Vector2::DrawInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const PropertyHint& hint)
	{
		UIElements::DragFloat2Field_U(name,
			[oracle, getter]() -> Variant
			{
				return getter(reinterpret_cast<Object*>(oracle()));
			},
			[oracle, setter](const Variant& value)
			{
				setter(reinterpret_cast<Object*>(oracle()), value);
			}
		);
	}

	void PrimitiveTypeInfo_Vector3::DrawInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const PropertyHint& hint)
	{
		UIElements::DragFloat3Field_U(name,
			[oracle, getter]() -> Variant
			{
				return getter(reinterpret_cast<Object*>(oracle()));
			},
			[oracle, setter](const Variant& value)
			{
				setter(reinterpret_cast<Object*>(oracle()), value);
			}
		);
	}

	void PrimitiveTypeInfo_Vector4::DrawInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const PropertyHint& hint)
	{
		UIElements::Color4Field_U(name,
			[oracle, getter]() -> Variant
			{
				return getter(reinterpret_cast<Object*>(oracle()));
			},
			[oracle, setter](const Variant& value)
			{
				setter(reinterpret_cast<Object*>(oracle()), value);
			}
		);
	}

	void PrimitiveTypeInfo_String::DrawInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const PropertyHint& hint)
	{
	}

	void RefTypeInfo::DrawInspectorGUI(const char* name, const Oracle& oracle, const PropertyGetter& getter, const PropertySetter& setter, const PropertyHint& hint)
	{
		UIElements::ObjectField_U(name, (ObjectTypeInfo*)TargetType,
			[oracle, getter]() -> Variant
			{
				return getter(reinterpret_cast<Object*>(oracle()));
			},
			[oracle, setter](const Variant& value)
			{
				setter(reinterpret_cast<Object*>(oracle()), value);
			}
		);
	}
}
