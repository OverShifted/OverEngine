#pragma once

namespace OverEngine
{
	enum class VariantType
	{
		Nil = 0,

		Bool,

		Int8,
		Int16,
		Int32,
		Int64,

		UInt8,
		UInt16,
		UInt32,
		UInt64,
		
		Float32,
		Float64,

		Vector2,
		Vector3,
		Vector4,

		ObjectRef
	};

	class Variant
	{
	private:
		VariantType m_Type;
		union {
			bool _Bool;

			int8_t _Int8;
			int16_t _Int16;
			int32_t _Int32;
			int64_t _Int64;

			uint8_t _UInt8;
			uint16_t _UInt16;
			uint32_t _UInt32;
			uint64_t _UInt64;

			float _Float32;
			double _Float64;

			Ref<Object> _ObjectRef;
		} m_Data;
	};
}
