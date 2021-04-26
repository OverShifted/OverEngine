#pragma once

#include "OverEngine/Core/Core.h"

#ifdef OE_PLATFORM_WINDOWS
	#include <Windows.h>

	#ifndef NOMINMAX
		// See github.com/skypjack/entt/wiki/Frequently-Asked-c4003-the-min-the-max-and-the-macro
		#define NOMINMAX
	#endif
#endif

#include <iostream>
#include <memory>
#include <thread>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "OverEngine/Core/Object.h"
#include "OverEngine/Core/String.h"
#include "OverEngine/Core/Log.h"
#include "OverEngine/Core/Runtime/Runtime.h"
#include "OverEngine/Core/Math/Math.h"
#include "OverEngine/Core/Math/Transform.h"

#include "OverEngine/Debug/Instrumentor.h"

