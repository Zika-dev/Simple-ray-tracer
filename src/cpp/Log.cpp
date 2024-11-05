#include "Log.hpp"

std::shared_ptr<spdlog::logger> Log::appLogger;

void Log::init() {

	spdlog::set_pattern("%^[%T] %n: %v%$");

	appLogger = spdlog::stdout_color_mt("APP");
	appLogger->set_level(spdlog::level::trace);
}