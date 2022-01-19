#include "MiraiLog.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>

#include <cstdio>
#include <atomic>

using namespace std;

MiraiLog* MiraiLog::get_instance()
{
    static MiraiLog log;
    return &log;
}

void MiraiLog::add_debug_log(const string& category, const string& dat)
{
    add_log(Level::DEBUG, category, dat);
}

void MiraiLog::add_info_log(const string& category, const string& dat)
{
    add_log(Level::INFO, category, dat);
}

void MiraiLog::add_warning_log(const string& category, const string& dat)
{
    add_log(Level::WARNING, category, dat);
}

void MiraiLog::add_fatal_log(const string& category, const string& dat)
{
    add_log(Level::FATAL, category, dat);
}

void MiraiLog::add_front_sinks(front_sinks_funtype front_sinks, void* user_dat)
{
    unique_lock<shared_mutex> lock(add_log_mx);
    front_sinks_vec.push_back({ front_sinks, user_dat });
}

void MiraiLog::add_backend_sinks(backend_sinks_funtype backend_sinks, void* user_dat)
{
    unique_lock<shared_mutex> lock(add_log_mx);
    backend_sinks_vec.push_back({ backend_sinks, user_dat });
}

MiraiLog::MiraiLog()
{
    spdlog::set_level(spdlog::level::debug);
    auto logger = spdlog::daily_logger_mt("MiraiLog", "log/daily.txt", 2, 30);
    logger->set_level(spdlog::level::debug);
}

/* 增加一条日志 */
void MiraiLog::add_log(const Level& lv, const string& category, const string& dat)
{
    shared_lock<shared_mutex> lock(add_log_mx);
    /* 调用所有前端sinks */
    pair<string, string> sinks_ret{ category ,dat };
    for (const auto& sinks : front_sinks_vec)
    {
        const auto slint_fun = sinks.first;
        const auto user_data = sinks.second;
        sinks_ret = slint_fun(lv, sinks_ret.first, sinks_ret.second, user_data);
    }

    /* 调用所有后端sinks */
    for (const auto& sinks : backend_sinks_vec)
    {
        const auto slint_fun = sinks.first;
        const auto user_data = sinks.second;
        slint_fun(lv, sinks_ret.first, sinks_ret.second, user_data);
    }

    /* 如果没有自定义的后端sinks,则调用默认sinks(打印到控制台上) */
    if (backend_sinks_vec.size() == 0)
    {
        default_backend_sinks(lv, sinks_ret.first, sinks_ret.second);
    }

}
/* 默认的后端sinks */
void MiraiLog::default_backend_sinks(const Level& lv, const string& category, const string& dat)
{
    auto daily_logger = spdlog::get("MiraiLog");
    switch (lv)
    {
    case Level::DEBUG:
        daily_logger->debug("[{}]:{}", category, dat);
        spdlog::debug("[{}]:{}", category, dat);
        break;
    case Level::INFO:
        daily_logger->info("[{}]:{}", category, dat);
        spdlog::info("[{}]:{}", category, dat);
        break;
    case Level::WARNING:
        daily_logger->warn("[{}]:{}", category, dat);
        spdlog::warn("[{}]:{}", category, dat);
        break;
    case Level::FATAL:
        daily_logger->critical("[{}]:{}", category, dat);
        spdlog::critical("[{}]:{}", category, dat);
        break;
    default:
        break;
    }
    daily_logger->flush();
}
