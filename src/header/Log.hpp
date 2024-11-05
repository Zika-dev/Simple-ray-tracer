#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

class Log
{
public:
	static void init();
	inline static std::shared_ptr<spdlog::logger>& getAppLogger() { return appLogger; }

private:
	static std::shared_ptr<spdlog::logger> appLogger;
};

// Log macros
#define APP_LOG_CRITICAL(...) Log::getAppLogger()->critical(__VA_ARGS__)
#define APP_LOG_ERROR(...) Log::getAppLogger()->error(__VA_ARGS__)
#define APP_LOG_WARN(...) Log::getAppLogger()->warn(__VA_ARGS__)
#define APP_LOG_INFO(...) Log::getAppLogger()->info(__VA_ARGS__)
#define APP_LOG_TRACE(...) Log::getAppLogger()->trace(__VA_ARGS__)