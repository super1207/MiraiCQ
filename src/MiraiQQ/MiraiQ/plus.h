#ifndef __PLUS_H__BBBKADOYHORUBXTX
#define __PLUS_H__BBBKADOYHORUBXTX

#include <string>
#include <vector>
#include <map>
#include <set>

#define WIN32_LEAN_AND_MEAN      // 在#include<windows.h>前定义
#include <Windows.h>  //for dll's HMODULE

#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/typeof/typeof.hpp> 
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include "Jsoncpp/json.h"




#define cq_funtype(x) cq_##x##_funtype
#define cq_funid(x,ID) cq_##x##_id = ID



class Plus
{
public:
	struct Event
	{
		__int32 id;
		__int32 type;
		std::string name;
		std::string function_name;
		__int32 priority;
		void * function_ptr;
	};

	struct Menu
	{
		std::string name;
		std::string function_name;
		void * function_ptr;
	};

	struct PlusDef
	{
		PlusDef()
		{
			is_enable = false;
			dll_ptr = NULL;
			ac = 0;
		}
		__int32 ac; //插件识别标记
		std::string plus_file_name;  //插件文件名字
		boost::filesystem::path plus_path; //插件路径，不包含文件名
		__int32 ret;
		__int32 apiver;
		std::string appid;
		std::string name;
		std::string version;
		__int32 version_id;
		std::string author;
		std::string description;
		std::vector<Event> event;
		std::vector<Menu> menu;
		std::vector<__int32> auth;
		std::string plus_json_str;// 插件的原始json串
		Json::Value json_value;
		bool is_enable; //插件是否被启用
		HMODULE dll_ptr;
		
	};

	boost::recursive_mutex mx;

	std::map<__int32,PlusDef> plus_map; //容纳插件的map

	


	typedef __int32(  __stdcall * cq_funtype(event_enable))();
	typedef __int32(  __stdcall * cq_funtype(event_disable))();
	typedef __int32(  __stdcall * cq_funtype(event_coolq_start))();
	typedef __int32(  __stdcall * cq_funtype(event_coolq_exit))();
	typedef __int32(  __stdcall * cq_funtype(event_private_message))(__int32 sub_type, __int32 msg_id, __int64 from_qq, const char *msg, __int32 font);
	typedef __int32(  __stdcall * cq_funtype(event_group_message))(__int32 sub_type, __int32 msg_id, __int64 from_group, __int64 from_qq, const char *from_anonymous_base64,const char *msg, __int32 font);
	typedef __int32(  __stdcall * cq_funtype(event_discuss_message))(__int32 sub_type, __int32 msg_id, __int64 from_discuss, __int64 from_qq, const char *msg, __int32 font);
	typedef __int32(  __stdcall * cq_funtype(event_group_upload))(__int32 sub_type, __int32 send_time, __int64 from_group, __int64 from_qq, const char *file_base64);
	typedef __int32(  __stdcall * cq_funtype(event_group_admin))(__int32 sub_type, __int32 send_time, __int64 from_group, __int64 being_operate_qq);
	typedef __int32(  __stdcall * cq_funtype(event_group_member_decrease))(__int32 sub_type, __int32 send_time, __int64 from_group, __int64 from_qq, __int64 being_operate_qq);
	typedef __int32(  __stdcall * cq_funtype(event_group_member_increase))(__int32 sub_type, __int32 send_time, __int64 from_group, __int64 from_qq, __int64 being_operate_qq);
	typedef __int32(  __stdcall * cq_funtype(event_group_ban))(__int32 sub_type, __int32 send_time, __int64 from_group, __int64 from_qq, __int64 being_operate_qq, __int64 duration);
	typedef __int32(  __stdcall * cq_funtype(event_friend_add))(__int32 sub_type, __int32 send_time, __int64 from_qq);
	typedef __int32(  __stdcall * cq_funtype(event_friend_request))(__int32 sub_type, __int32 send_time, __int64 from_qq, const char *msg, const char *response_flag);
	typedef __int32(  __stdcall * cq_funtype(event_group_request))(__int32 sub_type, __int32 send_time, __int64 from_group, __int64 from_qq, const char *msg, const char *response_flag);
	

	enum event_id_def 
	{
		cq_funid(event_enable,1003),
		cq_funid(event_disable,1004),
		cq_funid(event_coolq_start,1001),
		cq_funid(event_coolq_exit,1002),
		cq_funid(event_private_message,21),
		cq_funid(event_group_message,2),
		cq_funid(event_discuss_message,4),
		cq_funid(event_group_upload,11),
		cq_funid(event_group_admin,101),
		cq_funid(event_group_member_decrease,102),
		cq_funid(event_group_member_increase,103),
		cq_funid(event_group_ban,104),
		cq_funid(event_friend_add,201),
		cq_funid(event_friend_request,301),
		cq_funid(event_group_request,302)
	};

	Plus();

	/* 查找一个path下的所有插件 */
	static std::vector<boost::filesystem::path> find_plus_file(const boost::filesystem::path & path);

	/* 载入一个插件，默认不启用,返回ac号，如果返回0，则载入失败 */
	virtual __int32 add_plus(const boost::filesystem::path & path);

	/* 卸载一个插件 */
	virtual bool del_plus(__int32 ac);

	std::pair<bool,PlusDef> get_plusdef(__int32 ac);

	std::map<__int32,PlusDef> get_plus_map();


	/* 启用一个插件,若已经启用，则直接返回true */
	bool enable_plus(__int32 ac);

	/* 停用一个插件,若没有启用，则直接返回true */
	bool disable_plus(__int32 ac);

	static void * get_plus_function(const PlusDef & plus_def,__int32 id);
	

};


#undef cq_funtype
#undef cq_funid

#endif