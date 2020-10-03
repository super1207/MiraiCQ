#pragma once

#define WIN32_LEAN_AND_MEAN      // 在#include<windows.h>前定义
//#include <boost/asio.hpp>
#include <windows.h>

#include <string>
#include <boost/filesystem.hpp>
#include <vector>


#define MIRAI_OK 0
#define MIRAIQ_BOT_IS_CONNECT 1
#define MIRAIQ_BOT_NOT_CONNECT 2
#define MIRAIQ_URL_NOT_SET 3
#define MIRAIQ_LOAD_PLUS_ERR 4
#define MIRAIQ_PLUS_IS_ENABLE 5
#define MIRAIQ_PLUS_IS_NOT_ENABLE 6
#define MIRAI_PLUS_NOT_EXIST 7
#define MIRAI_FAILD 8

class MiraiQ
{

public:

	/* 是否处于连接状态 */
	virtual __int32 is_bot_connect();

	/* 设置url */
	virtual __int32 set_bot_url(const std::string & url);

	/* 获取url */
	virtual std::string get_bot_url();

	/* 连接 */
	virtual __int32 bot_connect();

	/* 断开连接 */
	virtual __int32 bot_disconnect();

	/* 加载一个插件 */
	virtual __int32 load_plus(const boost::filesystem::path & path);

	/* 获取插件的plus_id表 */
	virtual std::vector<__int32> get_plus_id_list(); 

	/* 启用一个插件 */
	virtual __int32 enable_plus(__int32 plus_id);

	/* 停用一个插件 */
	virtual __int32 disable_plus(__int32 plus_id);

	/* 查看一个插件是否启用 */
	virtual __int32 is_plus_enable(__int32 plus_id);

	/* 对所有插件调用 cq_start */
	virtual void call_cq_start_fun();

	/* 处理一条消息 */
	virtual __int32 deal_a_message();

	/* 获得Bot的指针 */
	static void * get_bot_ptr();

	/* 获得Plus的指针 */
	static void * get_plus_ptr();

	virtual ~MiraiQ(){};

	static MiraiQ * getInstance();
private:
	MiraiQ();

};