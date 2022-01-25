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
#include "ipc.h"

static std::map<std::string, std::pair<std::shared_ptr<std::condition_variable>, std::shared_ptr<std::string>>> g_api_map;
static std::mutex g_mx_api_map;
static thread_local std::string g_ret_str;

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

class EventRecvIPC
{
public:
	static EventRecvIPC* get_instance(const std::string & uuid)
	{
		static EventRecvIPC t(uuid);
		return &t;
	}
	std::string recv()
	{
		std::unique_lock<std::mutex> lock(mx_recv_list);
		cv.wait(lock, [&] {return recv_list.size() > 0;});
		std::string ret = (*recv_list.begin());
		recv_list.pop_front();
		return ret;
	}
	EventRecvIPC(const std::string & uuid)
	{
		flag = "EVENT" + uuid;
		std::thread([&]() {
			ipc::route cc{ flag.c_str(), ipc::receiver };
			while (true) {
				ipc::buff_t dd = cc.recv(); 
				std::string dat(dd.size(), '\0');
				memcpy_s((void*)dat.data(), dat.size(), dd.data(), dd.size());
				std::lock_guard<std::mutex> lock(mx_recv_list);
				if (recv_list.size() > max_recv_list) {
					recv_list.clear();
				}
				recv_list.push_back(dat);
				cv.notify_one();
			}
			}).detach();
	}
private:
	std::mutex mx_recv_list;
	std::list<std::string> recv_list;
	size_t max_recv_list = 1024;
	std::string flag;
	std::condition_variable cv;
};

class IPCSerClass
{
public:
	static IPCSerClass* getInstance(const std::string & uuid = "")
	{
		static IPCSerClass t(uuid);
		return &t;
	}
	std::string get_flag()
	{
		return uuid;
	}
	void send_event(const std::string & evt)
	{
		std::lock_guard<std::mutex> lock(mx_event_send_list);
		if (event_send_list.size() > max_send_list) {
			event_send_list.clear();
		}
		event_send_list.push_back(evt);
		cv_event.notify_one();
	}
	std::string get_api()
	{
		std::unique_lock<std::mutex> lock(mx_api_recv_list);
		cv_api.wait(lock, [&] {return api_recv_list.size() > 0;});
		std::string ret = (*api_recv_list.begin());
		api_recv_list.pop_front();
		return ret;
	}
	static bool send_api(const std::string &flag, const std::string& s)
	{
		std::string t = "API" + flag;
		ipc::channel cc{ t.c_str()};
		return cc.try_send(s);
	}
	static std::string get_event(const std::string& flag)
	{
		return EventRecvIPC::get_instance(flag)->recv();
	}
private:
	IPCSerClass(const std::string & uuid_)
	{
		if (uuid_ == "")
		{
			uuid = gen_uuid();
		}
		else
		{
			uuid = uuid_;
		}
		event_flag = "EVENT" + uuid;
		api_flag = "API" + uuid;

		std::thread([&]() {
			ipc::route cc{ event_flag.c_str() };
			++run_flag;
			while (true) {
				std::string to_send;
				{
					std::unique_lock<std::mutex> lock(mx_event_send_list);
					cv_event.wait(lock, [&] {return event_send_list.size() > 0;});
					to_send = (*event_send_list.begin());
					event_send_list.pop_front();
				}
				if (to_send != "")
				{
					cc.send(to_send);
				}
			}
		}).detach();
		std::thread([&]() {
			ipc::channel cc{ api_flag.c_str(), ipc::receiver };
			++run_flag;
			while (true) {
				ipc::buff_t dd = cc.recv();
				std::string dat(dd.size(), '\0');
				memcpy_s((void*)dat.data(), dat.size(), dd.data(), dd.size());
				std::lock_guard<std::mutex> lock(mx_api_recv_list);
				if (api_recv_list.size() > max_recv_list) {
					api_recv_list.clear();
				}
				api_recv_list.push_back(dat);
				cv_api.notify_all();
			}
		}).detach();
		while (true)
		{
			if (run_flag == 2)break;
		}
	}
	std::string uuid;
	std::string event_flag;
	std::string api_flag;
	std::atomic_int run_flag = 0;

	std::mutex mx_event_send_list;
	std::list<std::string> event_send_list;
	const size_t max_send_list = 1024;
	std::condition_variable cv_event;

	std::mutex mx_api_recv_list;
	std::list<std::string> api_recv_list;
	const size_t max_recv_list = 1024;
	std::condition_variable cv_api;
};


int IPC_Init(const char * uuid)
{
	if (!IPCSerClass::getInstance(uuid?uuid:""))
	{
		return -1;
	}
	return 0;
}

const char* IPC_GetFlag()
{
	g_ret_str = IPCSerClass::getInstance()->get_flag();
	return g_ret_str.c_str();
}

void IPC_SendEvent(const char* msg)
{
	if (!msg)
		return;
	IPCSerClass::getInstance()->send_event(msg);
}

const char* IPC_GetEvent(const char * flag)
{
	if (!flag)
	{
		return "";
	}
	g_ret_str =  IPCSerClass::get_event(flag);
	return g_ret_str.c_str();
}

void IPC_ApiRecv(void((*fun)(const char*, const char*, const char*)))
{
	
	std::string ret = IPCSerClass::getInstance()->get_api();
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
		fun(sender.c_str(), flag.c_str(), dat.c_str());
	}
}

void IPC_ApiReply(const char* sender, const char* flag, const char* msg)
{
	if (!sender || !flag)
	{
		return;
	}
	IPCSerClass::send_api(sender, std::string(flag) + (msg ? msg : ""));
}

const char* IPC_ApiSend(const char* remote, const char* msg,int tm)
{
	if (!remote || !msg)
	{
		return "";
	}
	std::string flag = gen_uuid(); //len:36
	std::string send_str = flag + "#" + flag + msg;
	
	std::atomic_bool is_run = false;
	std::string ret_dat;

	std::thread th([&]() {
		std::string t = "API" + flag;
		ipc::channel cc{ t.c_str(), ipc::receiver};
		is_run = true;
		ipc::buff_t dd = cc.recv(tm);
		if (!dd.empty())
		{
			ret_dat = (char *)dd.data();
		}
	});

	while (!is_run);
	IPCSerClass::send_api(remote, send_str);
	th.join();
	if (ret_dat.size() < 36)
	{
		return "";
	}
	g_ret_str =  ret_dat.substr(36);
	return g_ret_str.c_str();
}


