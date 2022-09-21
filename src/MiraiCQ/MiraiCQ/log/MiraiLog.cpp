#include "MiraiLog.h"
#include <jsoncpp/json.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>

#include <cstdio>
#include <atomic>
#include <algorithm>

#include "../tool/IPCTool.h"

using namespace std;

extern bool g_is_plus;
extern bool g_is_alone;
extern std::string g_plus_name;
extern std::string g_main_flag;

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

void MiraiLog::add_front_sinks(const std::string & flag,front_sinks_funtype front_sinks, void* user_dat)
{
    unique_lock<shared_mutex> lock(add_log_mx);
    front_sinks_vec.push_back({ flag,front_sinks, user_dat });
}

void MiraiLog::del_front_sinks(const std::string& flag)
{
    unique_lock<shared_mutex> lock(add_log_mx);
    auto iter = std::remove_if(front_sinks_vec.begin(), front_sinks_vec.end(), [&](const std::tuple<std::string, front_sinks_funtype, void*> & it) {
        return std::get<0>(it) == flag;
    });
    front_sinks_vec.erase(iter, front_sinks_vec.end());
}

void MiraiLog::add_backend_sinks(backend_sinks_funtype backend_sinks, void* user_dat)
{
    unique_lock<shared_mutex> lock(add_log_mx);
    backend_sinks_vec.push_back({ backend_sinks, user_dat });
}
static string get_exe_dir()
{
    string ret_str;
    char path_str[MAX_PATH + 1] = { 0 };
    const DWORD n = GetModuleFileNameA(NULL, path_str, MAX_PATH);
    if (n == 0)
    {
        ret_str = "";
    }
    else
    {
        *(strrchr(path_str, '\\')) = '\0';
        ret_str = path_str + string("\\");
    }
    return ret_str;
}
MiraiLog::MiraiLog()
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::flush_every(std::chrono::seconds(3));
    if (!g_is_alone)
    {
        if (!g_is_plus)
        {
            auto logger = spdlog::daily_logger_mt("MiraiLog", get_exe_dir() + "log/main_daily.txt", 2, 30);
            logger->set_level(spdlog::level::info);
            logger->flush_on(spdlog::level::info);
        }
        else {
            auto logger = spdlog::daily_logger_mt("MiraiLog", get_exe_dir() + "log/" + g_plus_name + "_plus_daily.txt", 2, 30);
            logger->set_level(spdlog::level::debug);
            logger->flush_on(spdlog::level::info);
        }
    }
}

/* 增加一条日志 */
void MiraiLog::add_log(const Level& lv, const string& category, const string& dat)
{
    shared_lock<shared_mutex> lock(add_log_mx);
    /* 调用所有前端sinks */
    pair<string, string> sinks_ret{ category ,dat };
    for (const auto& sinks : front_sinks_vec)
    {
        const auto slint_fun = std::get<1>(sinks);
        const auto user_data = std::get<2>(sinks);
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
    //if (backend_sinks_vec.size() == 0)
    //{
        default_backend_sinks(lv, sinks_ret.first, sinks_ret.second);
    //}

}


static std::string to_utf8(const std::string& ansi_str)
{
    int len = MultiByteToWideChar(CP_ACP, 0, ansi_str.c_str(), ansi_str.size(), NULL, 0);
    std::wstring unicode_buf(len, L'\0');
    MultiByteToWideChar(CP_ACP, 0, ansi_str.c_str(), ansi_str.size(), unicode_buf.data(), len);
    len = WideCharToMultiByte(CP_UTF8, 0, unicode_buf.data(), unicode_buf.size(), NULL, 0, NULL, NULL);
    std::string utf8_buf(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, unicode_buf.data(), unicode_buf.size(), utf8_buf.data(), len, NULL, NULL);
    return utf8_buf;
}


/* 默认的后端sinks */
void MiraiLog::default_backend_sinks(const Level& lv, const string& category, const string& dat)
{
    if (!g_is_alone) // 嵌入OlivOS的时候，不向文件和控制台输出日志
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
    }
    else {
        Json::Value send_json;
        send_json["action"] = "add_log";
        switch (lv)
        {
        case Level::DEBUG:
            send_json["params"]["level"] = "debug";
            break;
        case Level::INFO:
            send_json["params"]["level"] = "info";
            break;
        case Level::WARNING:
            send_json["params"]["level"] = "warning";
            break;
        case Level::FATAL:
            send_json["params"]["level"] = "fatal";
            break;
        default:
            break;
        }
        send_json["params"]["category"] = to_utf8(category);
        send_json["params"]["dat"] = to_utf8(dat);
        IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(send_json).c_str(), 5000);
    }
}
    
