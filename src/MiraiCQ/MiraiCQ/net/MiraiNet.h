#pragma once

#include <memory>
#include <string>
#include <vector>
#include <shared_mutex>
#include <map>
#include <mutex>
#include <atomic>

#include <jsoncpp/json.h>

class MiraiNet
{
public:
	using NetStruct = std::shared_ptr<Json::Value>;
	virtual ~MiraiNet();
	/*
	* 描述：连接net
	* 返回值：连接成功返回true，否则返回false
	*/
	virtual bool connect() = 0;

	/*
	* 描述：判断net是否连接
	* 返回值：已经连接成功返回true，否则返回false
	*/
	virtual bool is_connect() = 0;

	/*
	* 描述：调用net函数
	* 参数`senddat`：net参数
	* 参数`timeout`：超时，单位ms
	* 返回值：返回调用结果
	*/
	virtual NetStruct call_fun(NetStruct senddat,int timeout) = 0;
	
	/*
	* 描述：设置config，若已经存在则覆盖
	* 参数`key`：键
	* 参数`value`：值
	*/
	void set_config(const std::string & key, const std::string& value);

	/*
	* 描述：设置config，若已经存在则覆盖
	* 参数`key`：键
	* 参数`value`：值
	*/
	void set_all_config(const std::map<std::string, std::string> & all_config);

	/*
	* 描述：获得config
	* 参数`key`：键
	* 返回值：值，若键不存在则返回`""`
	*/
	std::string get_config(const std::string& key);

	/*
	* 描述：获得所有config
	* 返回值：所有config
	*/
	std::map<std::string, std::string> get_all_config() ;

	/*
	* 描述：获得事件，并清空事件数组
	* 返回值：事件数组
	*/
	std::vector<NetStruct> get_event();

	static std::shared_ptr<MiraiNet> get_instance(const std::string & type) ;
	
protected:
	MiraiNet();
	/*
	* 描述：添加事件到事件数组
	* 参数：事件
	*/
	void add_event(NetStruct event);

private:
	std::shared_mutex mx_config_map;
	std::map<std::string, std::string> config_map;
	std::mutex mx_event_vec;
	std::vector<NetStruct> event_vec;
	/* 记录event_vec的最大大小，若超过，则较旧的数据会被丢弃 */
	std::atomic_size_t buf_size = 128;
};
