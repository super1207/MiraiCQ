#include "MiraiLog.h"

#include <cstdio>

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
    lock_guard<mutex> lock(add_log_mx);
    front_sinks_vec.push_back({ front_sinks, user_dat });
}

void MiraiLog::add_backend_sinks(backend_sinks_funtype backend_sinks, void* user_dat)
{
    lock_guard<mutex> lock(add_log_mx);
    backend_sinks_vec.push_back({ backend_sinks, user_dat });
}

MiraiLog::MiraiLog()
{
}

/* 增加一条日志 */
void MiraiLog::add_log(const Level& lv, const string& category, const string& dat)
{
    lock_guard<mutex> lock(add_log_mx);

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
    static mutex mx;
    lock_guard<mutex> lock(mx);
    switch (lv)
    {
    case Level::DEBUG:
        printf("[DEBUG][%s]:%s\n", category.c_str(), dat.c_str());
        break;
    case Level::INFO:
        printf("[INFO][%s]:%s\n", category.c_str(), dat.c_str());
        break;
    case Level::WARNING:
        printf("[WARNING][%s]:%s\n", category.c_str(), dat.c_str());
        break;
    case Level::FATAL:
        printf("[FATAL][%s]:%s\n", category.c_str(), dat.c_str());
        break;
    default:
        break;
    }
}
