//
// Created by Aden on 10/28/2025.
//

#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/stdout_sinks-inl.h"

std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Log::s_RenderLogger;


void Log::Init()
{
    //start color [time format] logger name: Text - end color
    spdlog::set_pattern("%^[%T] %n: %v%$");
    s_CoreLogger = spdlog::stdout_color_mt("Core");
    s_CoreLogger->set_level(spdlog::level::trace);
    s_RenderLogger = spdlog::stdout_color_mt("Renderer");
    s_CoreLogger->set_level(spdlog::level::trace);


}
