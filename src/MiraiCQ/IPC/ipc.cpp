#include "pch.h"
#include <libipc/ipc.h>
#include <libipc/ipc.h>
#include <thread>
#include <list>
#include <mutex>
#include <map>
#include <Windows.h>
#include <objbase.h>
#include <condition_variable>
#include <chrono>

static std::map<std::string, std::pair<std::shared_ptr<std::condition_variable>,std::shared_ptr<std::string>>> g_api_map;
static std::mutex g_mx_api_map;
static thread_local std::string g_ret_str;

void* IPC_Malloc(size_t sz)
{
    return malloc(sz);
}

class EventSendIPC
{
public:
    static EventSendIPC* get_instance()
    {
        static EventSendIPC t;
        return &t;
    }
    void send(const std::string& s)
    {
        {
            std::lock_guard<std::mutex> lock(mx_send_list);
            send_list.push_back(s);
        }
        cv.notify_one();
    }
    std::string get_flag() const
    {
        return flag;
    }
    EventSendIPC()
    {
        flag = "MIRAICQ_EVENR_" + std::to_string(_getpid());
        std::thread([&]() {
            ipc::route cc{ flag.c_str() };
            while (true) {
                std::string to_send;
                {
                    std::unique_lock<std::mutex> lock(mx_send_list);
                    cv.wait(lock, [&] {return send_list.size() > 0;});
                    to_send = (*send_list.begin());
                    send_list.pop_front();
                }
                if (to_send != "")
                {
                    cc.send(to_send);
                }
            }
            }).detach();
    }
private:
    std::mutex mx_send_list;
    std::list<std::string> send_list;
    std::string flag;
    std::condition_variable cv;
};





class EventRecvIPC
{
public:
    static EventRecvIPC* get_instance(int center_pid)
    {
        static EventRecvIPC t(center_pid);
        return &t;
    }
    std::string recv()
    {
        std::lock_guard<std::mutex> lock(mx_recv_list);
        if (recv_list.size() == 0)
            return "";
        std::string ret = (*recv_list.begin());
        recv_list.pop_front();
        return ret;
    }
    std::string get_flag() const
    {
        return flag;
    }
    EventRecvIPC(int center_pid)
    {
        flag = "MIRAICQ_EVENR_" + std::to_string(center_pid);
        std::thread([&]() {
            ipc::route cc{ flag.c_str(), ipc::receiver };
            while (true) {
                ipc::buff_t dd = cc.recv();
                std::string dat(dd.size(), '\0');
                memcpy_s((void *)dat.data(), dat.size(), dd.data(), dd.size());
                std::lock_guard<std::mutex> lock(mx_recv_list);
                if (recv_list.size() > max_recv_list) {
                    recv_list.clear();
                }
                recv_list.push_back(dat);
            }
            }).detach();
    }
private:
    std::mutex mx_recv_list;
    std::list<std::string> recv_list;
    size_t max_recv_list = 1024;
    std::string flag;
};

class ApiRecvIPC
{
public:
    static ApiRecvIPC* get_instance()
    {
        static ApiRecvIPC t;
        return &t;
    }
    std::string recv()
    {
        std::lock_guard<std::mutex> lock(mx_recv_list);
        if (recv_list.size() == 0)
            return "";
        std::string ret = (*recv_list.begin());
        recv_list.pop_front();
        return ret;
    }
    std::string get_flag() const
    {
        return flag;
    }
    ApiRecvIPC()
    {
        flag = "MIRAICQ_API_" + std::to_string(_getpid());
        std::thread([&]() {
            ipc::channel cc{ flag.c_str(), ipc::receiver };
            while (true) {
                ipc::buff_t dd = cc.recv();
                std::string dat(dd.size(), '\0');
                memcpy_s((void *)dat.data(), dat.size(), dd.data(), dd.size());
                std::lock_guard<std::mutex> lock(mx_recv_list);
                if (recv_list.size() > max_recv_list) {
                    recv_list.clear();
                }
                recv_list.push_back(dat);
            }
            }).detach();
    }
private:
    std::mutex mx_recv_list;
    std::list<std::string> recv_list;
    size_t max_recv_list = 1024;
    std::string flag;
};


class ApiSendIPC
{
public:
    void send(const std::string& s)
    {
        flag = "MIRAICQ_API_" + std::to_string(pid);
        ipc::channel cc{ flag.c_str() };
        cc.send(s);
    }
    std::string get_flag() const
    {
        return flag;
    }
    ApiSendIPC(int pid)
    {
        flag = "MIRAICQ_API_" + std::to_string(pid);
        this->pid = pid;
    }
private:
    int pid = 0;
    std::string flag;
};

void IPC_SendEvent(const char* msg)
{
    if (!msg)
        return;
    EventSendIPC::get_instance()->send(msg);
}

const char* IPC_GetEvent(int pid)
{
    g_ret_str = EventRecvIPC::get_instance(pid)->recv();
    return g_ret_str.c_str();
}

void IPC_ApiRecv(char* ((*fun)(const char*)))
{
    std::string ret = ApiRecvIPC::get_instance()->recv();
    if (ret == "")
    {
        return;
    }
    size_t flaglen = 36;
    size_t pos = ret.find_first_of('#');
    std::string sender = ret.substr(0, pos);
    std::string flag = ret.substr(pos + 1, flaglen);
    std::string dat = ret.substr(pos + flaglen + 1);
    std::string ret_str;
    if (fun)
    {
        char* fun_ret = fun(dat.c_str());
        if (fun_ret)
        {
            ret_str = fun_ret;
            free(fun_ret);
        }
    }
    ApiSendIPC(std::stoi(sender)).send(flag + ret_str);
}

static std::string gen_uuid()
{
    char buf[64] = { 0 };
    GUID guid;
    if (CoCreateGuid(&guid) != S_OK)
    {
        return "";
    }
    sprintf_s(buf, 64,
        "%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1], guid.Data4[2],
        guid.Data4[3], guid.Data4[4], guid.Data4[5],
        guid.Data4[6], guid.Data4[7]);
    return buf;
}

const char* IPC_ApiSend(int to_pid, const char* msg)
{
    if (!msg)
    {
        return "";
    }
    std::string flag = gen_uuid(); //len:36
    static int this_pid = _getpid();
    std::string send_str = std::to_string(this_pid) + "#" + flag + msg;

    static std::mutex mx_is_first;
    static bool is_first = true;
    static std::atomic_bool is_run = false;;
    {
        std::lock_guard<std::mutex> lock(mx_is_first);
        if (is_first == true)
        {
            is_first = false;
            std::thread([&]() {
                while (true)
                {
                    std::string ret = ApiRecvIPC::get_instance()->recv();
                    is_run = true;
                    if (ret == "")
                    {
                        Sleep(15);
                        continue;
                    }
                    std::string flag = ret.substr(0, 36);
                    std::string dat = ret.substr(36);
                    std::lock_guard<std::mutex> lock(g_mx_api_map);
                    if (g_api_map.find(flag) != g_api_map.end())
                    {
                        g_api_map[flag].second = std::make_shared<std::string>(dat);
                        g_api_map[flag].first->notify_all();
                    }
                }
                }).detach();
        }
    }
    while (!is_run);

    std::shared_ptr<std::condition_variable> cv = std::make_shared<std::condition_variable>();
    {
        std::lock_guard<std::mutex> lock(g_mx_api_map);
        g_api_map[flag].first = cv;
        g_api_map[flag].second = nullptr;
    }

    ApiSendIPC(to_pid).send(send_str);
    std::shared_ptr<std::string> ret_ptr = nullptr;

    {
        using namespace std;
        std::unique_lock<std::mutex> lock(g_mx_api_map);
        if (cv->wait_for(lock, 15s, [&] {return (g_api_map[flag].second != nullptr);}))
        {
            ret_ptr = g_api_map[flag].second;
        }

    } 
    {
        std::lock_guard<std::mutex> lock(g_mx_api_map);
        g_api_map.erase(flag);
    }
    if (ret_ptr)
    {
        g_ret_str = (*ret_ptr);
        return g_ret_str.c_str();
    }
    return "";
}


