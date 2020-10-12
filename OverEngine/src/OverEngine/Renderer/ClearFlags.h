#pragma once

namespace OverEngine
{
	using ClearFlags = int8_t;

	enum ClearFlags_ : ClearFlags
	{
		ClearFlags_None = 0, ClearFlags_ClearColor = 1, ClearFlags_ClearDepth = 2
	};
}
