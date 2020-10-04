#pragma once

namespace OverEngine
{
	enum ClearFlags_ : uint8_t
	{
		ClearFlags_None = 0, ClearFlags_ClearColor = 1, ClearFlags_ClearDepth = 2
	};

	using ClearFlags = int8_t;
}
