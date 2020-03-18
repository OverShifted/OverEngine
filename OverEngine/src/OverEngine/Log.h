#pragma once

#include <memory>

#include "Core.h"
#include "spdlog/spdlog.h"

namespace OverEngine
{
	class OVER_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger;  }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger;  }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core log macros
#define OE_CORE_TRACE(...)     ::OverEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define OE_CORE_INFO(...)      ::OverEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define OE_CORE_WARN(...)      ::OverEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define OE_CORE_ERROR(...)     ::OverEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define OE_CORE_FATAL(...)     ::OverEngine::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define OE_TRACE(...)          ::OverEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define OE_INFO(...)           ::OverEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define OE_WARN(...)           ::OverEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define OE_ERROR(...)          ::OverEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define OE_FATAL(...)          ::OverEngine::Log::GetClientLogger()->fatal(__VA_ARGS__)