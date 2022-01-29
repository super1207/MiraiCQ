#include "pch.h"

#include <string>
#include <vector>
#include <thread>
#include <list>
#include <mutex>
#include <objbase.h>
#include <condition_variable>
#include <assert.h>
#include <shared_mutex>

#include "ipc.h"

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
	ZeroMemory(readBuff.data(), readBuff.size());
	while (ReadFile(hMailslot, readBuff.data(), readBuff.size(), &d, NULL) == FALSE) {
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
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
				MessageBoxA(NULL, ("--EventRecvIPC CreateMailslotA Failed" + std::to_string(GetLastError())).c_str(), "ERROR", MB_OK);
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
	void send_event(const std::string& evt)
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
	bool add_uuid(const std::string& uuid)
	{
		if (uuid.size() != 36) {
			return false;
		}
		std::unique_lock<std::shared_mutex> lock(mx_uuid_vec);
		uuid_vec.push_back(uuid);
		return true;
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

		std::thread([=]() {
			//ipc::route cc{ event_flag.c_str() };
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
					std::shared_lock<std::shared_mutex> lock(mx_uuid_vec);
					// 发给每一个客户端
					for (auto& id : uuid_vec) {
						std::string t = "EVENT" + id;
						AutoCloseHandle ah(CreateFileA(("\\\\.\\mailslot\\" + t).c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));
						HANDLE hMailslot = ah.get_handle();
						if (INVALID_HANDLE_VALUE == hMailslot) {
							continue;
						}
						DWORD d;
						WriteFile(hMailslot, to_send.data(), to_send.size(), &d, NULL);
					}
				}
			}
			}).detach();

			std::thread([=]() {
				AutoCloseHandle ah(CreateMailslotA(("\\\\.\\mailslot\\" + api_flag).c_str(), 0, MAILSLOT_WAIT_FOREVER, NULL));
				HANDLE hMailslot = ah.get_handle();
				if (INVALID_HANDLE_VALUE == hMailslot) {
					MessageBoxA(NULL, "CreateMailslotA Failed", "ERROR", MB_OK);
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

	std::vector<std::string> uuid_vec;
	std::shared_mutex mx_uuid_vec;
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
		catch (const std::exception&) {
			return -1;
		}

		return 0;
	}

	const char* IPC_GetFlag()
	{
		try {
			g_ret_str = IPCSerClass::getInstance()->get_flag();
			return g_ret_str.c_str();
		}
		catch (const std::exception&) {
			return "";
		}
	}

	void IPC_SendEvent(const char* msg)
	{
		if (!msg)
			return;
		try {
			IPCSerClass::getInstance()->send_event(msg);
		}
		catch (const std::exception&) {
			return;
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
		catch (const std::exception&) {
			return "";
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
		catch (const std::exception&) {
			return;
		}
	}

	int IPC_AddUUID(const char* uuid)
	{
		if (!uuid) {
			return -1;
		}
		bool ret = IPCSerClass::getInstance()->add_uuid(uuid);
		return (ret ? 0 : -1);
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
		catch (const std::exception&) {
			return;
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

			std::atomic_bool is_run = false;
			std::string ret_dat;

			std::thread th([&]() {
				std::string t = "API" + flag;
				AutoCloseHandle ah(CreateMailslotA(("\\\\.\\mailslot\\" + t).c_str(), 0, tm, NULL));
				HANDLE hMailslot = ah.get_handle();
				if (INVALID_HANDLE_VALUE == hMailslot) {
					MessageBoxA(NULL, "CreateMailslotA Failed", "ERROR", MB_OK);
					exit(-1);
				}
				is_run = true;
				ret_dat = read_sth_from_slot(hMailslot);
				});

			while (!is_run);
			IPCSerClass::send_api(remote, send_str);
			th.join();
			if (ret_dat.size() < 36)
			{
				return "";
			}
			g_ret_str = ret_dat.substr(36);
			return g_ret_str.c_str();
		}
		catch (const std::exception&) {
			return "";
		}
	}


#ifdef  __cplusplus
}
#endif


