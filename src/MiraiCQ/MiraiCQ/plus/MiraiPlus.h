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
			void* function = 0;
			int priority = 30000;
			std::string fun_name;
		};
		struct Menu
		{
			std::string name;
			void* function = 0;
		};
		void * module_hand = 0; /* dll模块的句柄 HMODULE */
		std::string name; /* 插件名字 */
		std::string filename; /* 插件的文件名 */
		std::string version; /* 插件版本 */
		std::string author; /* 插件作者 */
		std::string description; /* 插件描述 */
		int ac; /* 插件ac */
		std::vector<std::shared_ptr<const Event>> event_vec;
		std::vector<std::shared_ptr<const Menu>> menu_vec;
		std::set<int> auth_vec;
		std::atomic_bool is_enable = false; /* 插件是否启用 */
		std::atomic_bool is_first_enable = true; /* 第一次启用需要调用框架启动事件 */
		~PlusDef();
		/*
		* 描述：从插件中获得一个Event
		* 参数`type`：函数的type号
		* 返回值：成功返回Event，失败返回null
		*/
		const std::shared_ptr<const Event> get_event_fun(int type)noexcept;

		/*
		* 描述：从插件中获得Menu数组
		* 返回值：成功返回Menu数组，不会失败
		*/
		std::vector<std::shared_ptr<const Menu>> get_menu_vec() noexcept;

		/*
		* 描述：获得插件名字
		* 返回值：返回插件名字
		*/
		std::string get_name() noexcept;

		/*
		* 描述：获得插件名字
		* 返回值：返回插件名字
		*/
		std::string get_filename() noexcept;
	};

	~MiraiPlus();

	/*
	* 描述：加载一个dll插件
	* 参数`dll_name`:dll名字（不带后缀）
	* 参数`err_msg`,加载失败时说明原因
	* 返回值：成功返回`true`,失败返回`false`
	*/
	bool load_plus(const std::string& dll_name,std::string & err_msg) noexcept;

	/*
	* 描述：启用一个插件
	* 参数`ac`:插件标记
	* 参数`err_msg`,启用失败时说明原因
	* 返回值：返回函数执行后插件是否被启用
	*/
	bool enable_plus(int ac,std::string & err_msg) noexcept;

	/*
	* 描述：禁用一个插件
	* 参数`ac`:插件标记
	* 参数`err_msg`,禁用失败时说明原因
	* 返回值：返回函数执行后插件是否被禁用
	*/
	bool disable_plus(int ac, std::string & err_msg) noexcept;

	/*
	* 描述：判断一个插件是否被启用
	* 参数`ac`:插件标记
	* 返回值：已经被启用返回`true`，未启用或者ac错误返回false
	*/
	bool is_enable(int ac) noexcept;

	/*
	* 描述：卸载一个插件
	* 参数`ac`:插件标记
	* 参数`err_msg`,卸载失败时说明原因
	* 返回值：返回函数执行后插件是否被卸载
	*/
	bool del_plus(int ac) noexcept;

	/*
	* 描述：通过AC来获得一个插件
	* 参数`ac`:插件标记
	* 返回值：成功返回插件指针，ac无效返回null
	*/
	std::shared_ptr<PlusDef> get_plus(int ac) noexcept;

	/*
	* 描述：获得插件map
	* 返回值：返回插件map指针，此函数不会失败
	*/
	std::map<int, std::shared_ptr<PlusDef>> get_all_plus() noexcept;

	/*
	* 描述：获得插件ac
	* 返回值：返回ac数组，此函数不会失败
	*/
	std::vector<std::pair<int, std::weak_ptr<MiraiPlus::PlusDef>>> get_all_ac() noexcept;

	static MiraiPlus* get_instance() noexcept;
private:
	MiraiPlus();
	std::map<int, std::shared_ptr<PlusDef>> plus_map;
	std::shared_mutex mx_plus_map;

};

