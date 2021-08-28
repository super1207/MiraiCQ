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
	* ��������ȡLog�ӿ�
	* ����ֵ������`MiraiLog`ʵ��,��ʧ���򷵻�`nullptr`
	*/
	static MiraiLog * get_instance();

	/*
	* ����������debug��־
	* ����`category`����־����
	* ����`dat`����־����
	*/
	void add_debug_log(const std::string& category, const std::string& dat);

	/*
	* ����������info��־
	* ����`category`����־����
	* ����`dat`����־����
	*/
	void add_info_log(const std::string& category, const std::string& dat);

	/*
	* ����������warning��־
	* ����`category`����־����
	* ����`dat`����־����
	*/
	void add_warning_log(const std::string& category, const std::string& dat);

	/*
	* ����������fatal��־
	* ����`category`����־����
	* ����`dat`����־����
	*/
	void add_fatal_log(const std::string& category, const std::string& dat);

	/*
	* ����������ǰ��sinks
	* ����`front_sinks`��ǰ��sinks����
	* ����`user_dat`���û�����ָ��
	*/
	void add_front_sinks(front_sinks_funtype front_sinks,void * user_dat);

	/*
	* ���������Ӻ��sinks
	* ����`backend_sinks`�����sinks����
	* ����`user_dat`���û�����ָ��
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
