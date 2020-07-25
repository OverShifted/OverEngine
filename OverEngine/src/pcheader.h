#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <iostream>
#include <memory>
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

#include "OverEngine/Core/Math/Math.h"
#include "OverEngine/Core/Math/Transform.h"
#include "OverEngine/Core/Log.h"
#include "OverEngine/Core/Core.h"

#include "OverEngine/Debug/Instrumentor.h"

#ifdef OE_PLATFORM_WINDOWS
#include <Windows.h>
#endif // OE_PLATFORM_WINDOWS