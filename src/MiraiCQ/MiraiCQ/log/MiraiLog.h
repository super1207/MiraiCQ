#pragma once

#include <string>
#include <mutex>
#include <functional>
#include <vector>

class MiraiLog
{
public:
	enum class Level
	{
		DEBUG,
		INFO,
		WARNING,
		FATAL
	};
	using front_sinks_funtype = std::function<std::pair<std::string,std::string>(const Level & lv,const std::string& category, const std::string& dat, void* user_ptr)>;
	using backend_sinks_funtype = std::function<void(const Level& lv, const std::string& category, const std::string& dat, void* user_ptr)>;

public:
	/*
	* 描述：获取Log接口
	* 返回值：返回`MiraiLog`实例,若失败则返回`nullptr`
	*/
	static MiraiLog * get_instance();

	/*
	* 描述：增加debug日志
	* 参数`category`：日志种类
	* 参数`dat`：日志内容
	*/
	void add_debug_log(const std::string& category, const std::string& dat);

	/*
	* 描述：增加info日志
	* 参数`category`：日志种类
	* 参数`dat`：日志内容
	*/
	void add_info_log(const std::string& category, const std::string& dat);

	/*
	* 描述：增加warning日志
	* 参数`category`：日志种类
	* 参数`dat`：日志内容
	*/
	void add_warning_log(const std::string& category, const std::string& dat);

	/*
	* 描述：增加fatal日志
	* 参数`category`：日志种类
	* 参数`dat`：日志内容
	*/
	void add_fatal_log(const std::string& category, const std::string& dat);

	/*
	* 描述：增加前端sinks
	* 参数`front_sinks`：前端sinks函数
	* 参数`user_dat`：用户数据指针
	*/
	void add_front_sinks(front_sinks_funtype front_sinks,void * user_dat);

	/*
	* 描述：增加后端sinks
	* 参数`backend_sinks`：后端sinks函数
	* 参数`user_dat`：用户数据指针
	*/
	void add_backend_sinks(backend_sinks_funtype backend_sinks, void* user_dat);

private:
	MiraiLog();
	void add_log(const Level& lv, const std::string& category, const std::string& dat);
	void default_backend_sinks(const Level& lv, const std::string& category, const std::string& dat);

private:
	std::vector<std::pair<front_sinks_funtype,void *>> front_sinks_vec;
	std::vector<std::pair<backend_sinks_funtype, void*>> backend_sinks_vec;
	std::mutex add_log_mx;
	
};

