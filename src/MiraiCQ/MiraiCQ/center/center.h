#pragma once

#include <memory>
#include <string>
#include <atomic>
#include <functional>

#include "../plus/MiraiPlus.h"
#include "../net/MiraiNet.h"

class Center
{
public:
	struct PlusInfo
	{
		int ac;
		std::string name;
		std::string version;
		std::string description;
		std::string author;
		bool is_enable;
		bool is_load;
	};
	static Center * get_instance() ;
	~Center();
	/**************************来自export的调用**********************************/
	/* 初始化Center */
	void set_net(std::weak_ptr<MiraiNet> net) ;
	//bool re_connect() ;
	/* 加载所有插件,返回加载成功的插件数量 */
	int load_all_plus() ;
	/* 启动所有插件，返回启动成功的插件数量 */
	int enable_all_plus() ;
	/* 卸载所有插件，返回卸载成功的插件数量 */
	int del_all_plus() ;
	/* 运行Center */
	bool run() ;
	/* 获取所有插件的ac */
	std::vector<int> get_ac_vec() ;
	/* 通过ac获得插件信息 */
	std::shared_ptr<PlusInfo> get_plus_by_ac(int ac) ;
	std::string get_menu_name_by_ac(int ac, int pos);
	void call_menu_fun_by_ac(int ac, int pos);
	/**************************来自插件的调用**********************************/
	int CQ_sendPrivateMsg(int auth_code, int64_t qq, const char* msg) ;
	int CQ_sendGroupMsg(int auth_code, int64_t group_id, const char* msg) ;
	int CQ_sendDiscussMsg(int auth_code, int64_t discuss_id, const char* msg);
	int CQ_deleteMsg(int auth_code, int64_t msg_id) ;
	int CQ_sendLike(int auth_code, int64_t qq) ;
	int CQ_sendLikeV2(int auth_code, int64_t qq, int times);
	int CQ_setGroupKick(int auth_code, int64_t group_id, int64_t qq, int reject_add_request) ;
	int CQ_setGroupBan(int auth_code, int64_t group_id, int64_t qq, int64_t duration) ;
	int CQ_setGroupAnonymousBan(int auth_code, int64_t group_id, const char* anonymous, int64_t duration) ;
	int CQ_setGroupWholeBan(int auth_code, int64_t group_id,int enable) ;
	int CQ_setGroupAdmin(int auth_code, __int64 group_id, __int64 qq, int set) ;
	int CQ_setGroupAnonymous(int auth_code, __int64 group_id, int enable);
	int CQ_setGroupCard(int auth_code, __int64 group_id, __int64 qq, const char* new_card);
	int CQ_setGroupLeave(int auth_code, __int64 group_id, int is_dismiss);
	int CQ_setGroupSpecialTitle(int auth_code, __int64 group_id, __int64 qq, const char* new_special_title, __int64 duration);
	int CQ_setFriendAddRequest(__int32 auth_code, const char* response_flag, __int32 response_operation, const char* remark);
	int CQ_setGroupAddRequest(__int32 auth_code, const char* response_flag, int request_type, int response_operation);
	int CQ_setGroupAddRequestV2(__int32 auth_code, const char* response_flag, int request_type, int response_operation, const char* reason);
	int64_t CQ_getLoginQQ(int auth_code);
	std::string CQ_getLoginNick(int auth_code);
	std::string CQ_getStrangerInfo(int auth_code, __int64 qq, int no_cache);
	std::string CQ_getFriendList(int auth_code, int reserved);
	std::string CQ_getGroupList(int auth_code);
	std::string CQ_getGroupInfo(int auth_code, int64_t group_id, int no_cache);
	std::string CQ_getGroupMemberList(int auth_code, int64_t group_id);
	std::string CQ_getGroupMemberInfoV2(int auth_code, int64_t group_id, int64_t qq, int no_cache);
	std::string CQ_getCookies(int auth_code);
	std::string CQ_getCookiesV2(int auth_code, const char* domain);
	int CQ_getCsrfToken(int auth_code);
	std::string CQ_getAppDirectory(int auth_code);
	std::string CQ_getRecord(int auth_code, const char* file, const char* out_format);
	std::string CQ_getRecordV2(int auth_code, const char* file, const char* out_format);
	std::string CQ_getImage(int auth_code, const char* file);
	int CQ_canSendImage(int auth_code);
	int CQ_canSendRecord(int auth_code);
	int  CQ_addLog(int auth_code, int log_level, const char* category, const char* log_msg);
	int  CQ_setFatal(__int32 auth_code, const char* error_info);
	int  CQ_setRestart(int auth_code);
	std::string CQ_callApi(int auth_code, const char* msg);
	int CQ_setDiscussLeave(int auth_code, int64_t discuss_id);

private:
	std::weak_ptr<MiraiNet> net;
	std::shared_mutex mx_net; /* 用于对智能指针本身进行加锁 */

	std::atomic_bool is_run = false;
	std::atomic_bool can_run = false;
private:
	Center();
	void deal_event(MiraiNet::NetStruct evt) ;
	void deal_type_message(Json::Value& evt);
	void deal_type_notice(Json::Value& evt);
	void deal_type_notice_group_upload(Json::Value& evt);
	void deal_type_notice_group_admin(Json::Value& evt);
	void deal_type_notice_group_decrease(Json::Value& evt);
	void deal_type_notice_group_increase(Json::Value& evt);
	void deal_type_notice_group_ban(Json::Value& evt);
	void deal_type_notice_friend_add(Json::Value& evt);
	void deal_type_request_friend(Json::Value& evt);
	void deal_type_request_group(Json::Value& evt);
	void deal_type_request(Json::Value& evt);
	void deal_type_meta_event(Json::Value& evt);
	void deal_type_message_private(Json::Value& evt);
	void deal_type_message_group(Json::Value& evt);
	void deal_1207_event(Json::Value& evt);

};

