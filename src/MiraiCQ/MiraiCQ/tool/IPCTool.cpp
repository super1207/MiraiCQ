#include <string>
#include <vector>
#include <thread>
#include <list>
#include <mutex>
#include <objbase.h>
#include <condition_variable>
#include <assert.h>
#include <shared_mutex>
#include "./IPCTool.h"
#include "../log/MiraiLog.h"

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
static std::string read_sth_from_slot(HANDLE hMailslot)
{
	DWORD d;
	std::string readBuff = std::string(1024, '\0');
	while (ReadFile(hMailslot, readBuff.data(), readBuff.size(), &d, NULL) == FALSE) {
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			ZeroMemory(readBuff.data(), readBuff.size());
			readBuff.resize(readBuff.size() * 2);
		}
		else {
			return "";
		}
	}
	return readBuff;

}

/* 用于自动关闭handle */
class AutoCloseHandle {
public:
	AutoCloseHandle(HANDLE handle) :handle_t(handle) {

	}
	~AutoCloseHandle() {
		if (handle_t != INVALID_HANDLE_VALUE) {
			CloseHandle(handle_t);
		}
	}
	HANDLE get_handle() {
		return handle_t;
	}
private:
	HANDLE handle_t;
};

class EventRecvIPC
{
public:
	static EventRecvIPC* get_instance(const std::string& uuid)
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
private:
	EventRecvIPC(const std::string& uuid)
	{
		flag = "EVENT" + uuid;
		std::thread([&]() {
			AutoCloseHandle ah(CreateMailslotA(("\\\\.\\mailslot\\" + flag).c_str(), 0, MAILSLOT_WAIT_FOREVER, NULL));
			HANDLE hMailslot = ah.get_handle();
			if (INVALID_HANDLE_VALUE == hMailslot) {
				MiraiLog::get_instance()->add_fatal_log("EventRecvIPC", "CreateMailslotA Failed" + std::to_string(GetLastError()));
				exit(-1);
			}
			while (true) {
				std::string tt = read_sth_from_slot(hMailslot);
				std::lock_guard<std::mutex> lock(mx_recv_list);
				if (recv_list.size() > max_recv_list) {
					recv_list.clear();
				}
				recv_list.push_back(tt);
				cv.notify_one();
			}
			}).detach();
	}
	std::mutex mx_recv_list;
	std::list<std::string> recv_list;
	size_t max_recv_list = 1024;
	std::string flag;
	std::condition_variable cv;
};

class IPCSerClass
{
public:
	static IPCSerClass* getInstance(const std::string& uuid = "")
	{
		static IPCSerClass t(uuid);
		return &t;
	}
	std::string get_flag()
	{
		return uuid;
	}
	void send_event(const std::string & uuid,const std::string& evt)
	{

		AutoCloseHandle ah(CreateFileA(("\\\\.\\mailslot\\EVENT" + uuid).c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));
		HANDLE hMailslot = ah.get_handle();
		if (INVALID_HANDLE_VALUE == hMailslot) {
			return;
		}
		DWORD d;
		WriteFile(hMailslot, evt.data(), evt.size(), &d, NULL);
	}
	std::string get_api()
	{
		std::unique_lock<std::mutex> lock(mx_api_recv_list);
		cv_api.wait(lock, [&] {return api_recv_list.size() > 0;});
		std::string ret = (*api_recv_list.begin());
		api_recv_list.pop_front();
		return ret;
	}

	static bool send_api(const std::string& flag, const std::string& s)
	{
		std::string t = "API" + flag;
		AutoCloseHandle ah(CreateFileA(("\\\\.\\mailslot\\" + t).c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));
		HANDLE hMailslot = ah.get_handle();
		if (INVALID_HANDLE_VALUE == hMailslot) {
			return false;
		}
		DWORD d;
		if (!WriteFile(hMailslot, s.data(), s.size(), &d, NULL)) {
			return false;
		}
		return  true;
	}
	static std::string get_event(const std::string& flag)
	{
		return EventRecvIPC::get_instance(flag)->recv();
	}
private:
	IPCSerClass(const std::string& uuid_)
	{
		if (uuid_ == "")
		{
			uuid = gen_uuid();
		}
		else
		{
			uuid = uuid_;
		}
		//event_flag = "EVENT" + uuid;
		api_flag = "API" + uuid;
		std::thread([&]() {
			AutoCloseHandle ah(CreateMailslotA(("\\\\.\\mailslot\\" + api_flag).c_str(), 0, MAILSLOT_WAIT_FOREVER, NULL));
			HANDLE hMailslot = ah.get_handle();
			if (INVALID_HANDLE_VALUE == hMailslot) {
				MiraiLog::get_instance()->add_fatal_log("IPCSerClass", "CreateMailslotA Failed" + std::to_string(GetLastError()));
				exit(-1);
			}

			++run_flag;
			while (true) {
				std::string tt = read_sth_from_slot(hMailslot);
				std::lock_guard<std::mutex> lock(mx_api_recv_list);
				if (api_recv_list.size() > max_recv_list) {
					api_recv_list.clear();
				}
				api_recv_list.push_back(tt);
				cv_api.notify_all();
			}
			}).detach();


		while (true)
		{
			if (run_flag == 1)break;
			Sleep(10);
		}
	}
	std::string uuid;
	std::string event_flag;
	std::string api_flag;
	std::atomic_int run_flag = 0;

	std::mutex mx_api_recv_list;
	std::list<std::string> api_recv_list;
	const size_t max_recv_list = 1024;
	std::condition_variable cv_api;
};

#ifdef  __cplusplus
extern "C" {
#endif

	int IPC_Init(const char* uuid)
	{
		try
		{
			if (!IPCSerClass::getInstance(uuid ? uuid : ""))
			{
				return -1;
			}
		}
		catch (const std::exception& e) {
			MiraiLog::get_instance()->add_fatal_log("IPC_Init", "未知异常 in IPC_Init：" + std::string(e.what()));
			exit(-1);
		}

		return 0;
	}

	const char* IPC_GetFlag()
	{
		try {
			g_ret_str = IPCSerClass::getInstance()->get_flag();
			return g_ret_str.c_str();
		}
		catch (const std::exception& e) {
			MiraiLog::get_instance()->add_fatal_log("IPC_GetFlag", "未知异常 in IPC_GetFlag：" + std::string(e.what()));
			exit(-1);
		}
	}

	void IPC_SendEvent(const char * uuid,const char* msg)
	{
		if (!msg)
			return;
		try {
			IPCSerClass::getInstance()->send_event(uuid,msg);
		}
		catch (const std::exception& e) {
			MiraiLog::get_instance()->add_fatal_log("IPC_SendEvent", "未知异常 in IPC_SendEvent：" + std::string(e.what()));
			exit(-1);
		}
	}

	const char* IPC_GetEvent(const char* flag)
	{
		if (!flag)
		{
			return "";
		}
		try {
			g_ret_str = IPCSerClass::get_event(flag);
			return g_ret_str.c_str();
		}
		catch (const std::exception& e) {
			MiraiLog::get_instance()->add_fatal_log("IPC_GetEvent", "未知异常 in IPC_GetEvent：" + std::string(e.what()));
			exit(-1);
		}
	}

	void IPC_ApiRecv(void((*fun)(const char* sender_uuid, const char* flag, const char* msg)))
	{
		try {
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
		catch (const std::exception& e) {
			MiraiLog::get_instance()->add_fatal_log("IPC_ApiRecv", "未知异常 in IPC_ApiRecv：" + std::string(e.what()));
			exit(-1);
		}
	}

	void IPC_ApiReply(const char* sender, const char* flag, const char* msg)
	{
		try {
			if (!sender || !flag)
			{
				return;
			}
			IPCSerClass::send_api(sender, std::string(flag) + (msg ? msg : ""));
		}
		catch (const std::exception& e) {
			MiraiLog::get_instance()->add_fatal_log("IPC_ApiReply", "未知异常 in IPC_ApiReply：" + std::string(e.what()));
			exit(-1);
		}
	}

	const char* IPC_ApiSend(const char* remote, const char* msg, int tm)
	{
		if (!remote || !msg)
		{
			return "";
		}
		try {
			std::string flag = gen_uuid(); //len:36
			std::string send_str = flag + "#" + IPC_GetFlag() + msg;

			bool is_run = false;
			std::string ret_dat;

			std::condition_variable cv;
			std::mutex mx;

			std::thread th([&]() {
				std::string t = "API" + flag;
				AutoCloseHandle ah(CreateMailslotA(("\\\\.\\mailslot\\" + t).c_str(), 0, tm, NULL));
				HANDLE hMailslot = ah.get_handle();
				if (INVALID_HANDLE_VALUE == hMailslot) {
					MiraiLog::get_instance()->add_fatal_log("IPC_ApiSend", "CreateMailslotA Error:" + std::to_string(GetLastError()));
					exit(-1);
				}
				{
					std::lock_guard<std::mutex> lk(mx);
					is_run = true;
					cv.notify_one();
				}
				
				ret_dat = read_sth_from_slot(hMailslot);
			});

			/* 等待slot创建 */
			{
				std::unique_lock<std::mutex> lock(mx);
				cv.wait(lock, [&] {return is_run == true;});
			}
			
			IPCSerClass::send_api(remote, send_str);
			th.join();
			if (ret_dat.size() < 36)
			{
				return "";
			}
			g_ret_str = ret_dat.substr(36);
			return g_ret_str.c_str();
		}
		catch (const std::exception& e) {
			MiraiLog::get_instance()->add_fatal_log("IPC_ApiSend", "未知异常 in IPC_ApiSend：" + std::string(e.what()));
			exit(-1);
		}
	}


#ifdef  __cplusplus
}
#endif


