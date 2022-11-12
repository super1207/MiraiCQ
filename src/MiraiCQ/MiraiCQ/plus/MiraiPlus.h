#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include <shared_mutex>
#include <atomic>

class MiraiPlus
{
public:

	struct PlusDef
	{
		struct Event
		{
			int type;
			int priority = 30000;
			std::string fun_name;
		};
		struct Menu
		{
			std::string fun_name;
			std::string name;
		};
		struct Process
		{
			Process(const std::string& dll_name,const std::string & uuid);
			/* 判断进程是否存在 */
			bool is_exist();
			/* 等待进程退出,返回函数执行完后进程是否退出 */
			void wait_process_quit(int timeout);
			/* 获得进程id */
			int get_pid();
			~Process();
		private:
			void* process_handle = NULL;
			void* job_handle = NULL;
		};
		std::string name; /* 插件名字 */
		std::string filename; /* 插件的文件名 */
		std::string version; /* 插件版本 */
		std::string author; /* 插件作者 */
		std::string description; /* 插件描述 */
		std::string dll_name;  /* 插件的文件名(不含后缀) */
		int ac; /* 插件ac */
		std::vector<std::shared_ptr<const Event>> event_vec;
		std::vector<std::shared_ptr<const Menu>> menu_vec;
		// std::set<int> auth_vec;
		std::shared_ptr<Process> process = nullptr; /* 插件的进程 */
		std::shared_mutex mx_plus_def;
		bool recive_ex_event = false;

		~PlusDef();
		/*
		* 描述：从插件中获得一个Event
		* 参数`type`：函数的type号
		* 返回值：成功返回Event，失败返回null
		*/
		const std::shared_ptr<const Event> get_event_fun(int type);

		/*
		* 描述：从插件中获得Menu数组
		* 返回值：成功返回Menu数组，不会失败
		*/
		std::vector<std::shared_ptr<const Menu>> get_menu_vec() ;

		/*
		* 描述：获得插件名字
		* 返回值：返回插件名字
		*/
		std::string get_name() ;

		/*
		* 描述：获得插件名字
		* 返回值：返回插件名字
		*/
		std::string get_filename() ;

		/*
		* 描述：返回插件uuid，uuid可用于与插件进行IPC通讯，若插件未启用，则返回""
		* 返回值：返回插件uuid
		*/
		std::string get_uuid();

		/*
		* 描述：设置插件的uuid
		*/
		void set_uuid(const std::string & uuid);

		/*
		* 描述：返回插件是否启用,注意，并不是插件启用，插件进程就一定存在，因为插件可能异常退出
		*/
		bool is_enable();

		/*
		*  描述：返回插件进程是否存在，注意，若插件没有启用，则插件进程一定不存在
		*/
		bool is_process_exist();

		/*
		*  描述：返回插件进程的pid，若插件进程不存在，则返回-1
		*/
		int get_process_id();

		bool is_recive_ex_event();
	private:
		std::string uuid;
	};

	~MiraiPlus();

	/*
	* 描述：加载一个dll插件
	* 参数`dll_name`:dll名字（不带后缀）
	* 参数`err_msg`,加载失败时说明原因
	* 返回值：成功返回`true`,失败返回`false`
	*/
	bool load_plus(const std::string& dll_name,std::string & err_msg) ;

	/*
	* 描述：启用一个插件
	* 参数`ac`:插件标记
	* 参数`err_msg`,启用失败时说明原因
	* 返回值：返回函数执行后插件是否被启用
	*/
	bool enable_plus(int ac,std::string & err_msg) ;

	/*
	* 描述：禁用一个插件
	* 参数`ac`:插件标记
	*/
	void disable_plus(int ac) ;

	/*
	* 描述：判断一个插件是否被启用
	* 参数`ac`:插件标记
	* 返回值：已经被启用返回`true`，未启用或者ac错误返回false
	*/
	bool is_enable(int ac) ;

	/*
	* 描述：通过AC来获得一个插件
	* 参数`ac`:插件标记
	* 返回值：成功返回插件指针，ac无效返回null
	*/
	std::shared_ptr<PlusDef> get_plus(int ac) ;

	/*
	* 描述：获得插件map
	* 返回值：返回插件map指针，此函数不会失败
	*/
	std::map<int, std::shared_ptr<PlusDef>> get_all_plus() ;

	/*
	* 描述：获得插件ac
	* 返回值：返回ac数组，此函数不会失败
	*/
	std::vector<std::pair<int, std::weak_ptr<MiraiPlus::PlusDef>>> get_all_ac() ;

	static MiraiPlus* get_instance() ;
private:
	MiraiPlus();
	std::map<int, std::shared_ptr<PlusDef>> plus_map;
	std::shared_mutex mx_plus_map;
};

