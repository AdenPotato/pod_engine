#pragma once
#include "podpch.h"

#include "Core/Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

class Log
{
public:
    static void Init();

    inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
    inline static std::shared_ptr<spdlog::logger>& GetRenderLogger() { return s_RenderLogger; }

private:

    static std::shared_ptr<spdlog::logger> s_CoreLogger;
    static std::shared_ptr<spdlog::logger> s_RenderLogger;
};

//core macro
#define CORE_LOGGER_ERROR(...) ::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CORE_LOGGER_WARN(...) ::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CORE_LOGGER_INFO(...) ::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CORE_LOGGER_DEBUG(...) ::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define CORE_LOGGER_TRACE(...) ::Log::GetCoreLogger()->trace(__VA_ARGS__)

//render marco
#define RENDERER_LOGGER_ERROR(...) ::Log::GetRenderLogger()->error(__VA_ARGS__)
#define RENDERER_LOGGER_WARN(...) ::Log::GetRenderLogger()->warn(__VA_ARGS__)
#define RENDERER_LOGGER_INFO(...) ::Log::GetRenderLogger()->info(__VA_ARGS__)
#define RENDERER_LOGGER_DEBUG(...) ::Log::GetRenderLogger()->debug(__VA_ARGS__)
#define RENDERER_LOGGER_TRACE(...) ::Log::GetRenderLogger()->trace(__VA_ARGS__)

