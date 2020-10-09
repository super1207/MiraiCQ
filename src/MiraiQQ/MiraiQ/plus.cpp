#include "plus.h"
#include <fstream>

static __int32 g_ac = 1; 

#define cq_funtype(x) cq_##x##_funtype

Plus::Plus()
{

}

std::vector<boost::filesystem::path> Plus::find_plus_file(const boost::filesystem::path & path)
{

	//得到所有文件
	std::vector<boost::filesystem::path> ret_vec;
	boost::filesystem::directory_iterator end_it;
	if (!( boost::filesystem::exists(path) && 
		boost::filesystem::is_directory(path)))
	{
		if(!boost::filesystem::create_directory(path))
		{
			return ret_vec;
		}
	}
	for (boost::filesystem::directory_iterator iter(path); iter != end_it; iter++) {
		if (!boost::filesystem::is_directory(*iter)) {
			std::string file_name =  iter->path().string();
			if(boost::to_lower_copy(file_name) == "cqp.dll")
			{
				continue;
			}
			if(boost::ends_with(boost::to_lower_copy(file_name),".dll"))
			{
				//返回绝对路径
				ret_vec.push_back(boost::filesystem::absolute(*iter));
			}

		} 
	}
	return ret_vec;
}


__int32 Plus::add_plus( const boost::filesystem::path & path )
{
	mx.lock();
	boost::filesystem::path parent_path = path.parent_path();
	std::string file_stem = path.stem().string();

	boost::filesystem::path json_path = parent_path;
	json_path /= (file_stem + ".json");

	//读取json文件
	PlusDef plusdef;

	std::locale loc= std::locale::global(std::locale(""));
	std::ifstream in(json_path.string().c_str(),std::ios::binary);
	std::locale::global(loc);
	if (!in.is_open())
	{
		BOOST_LOG_TRIVIAL(info) << "jsonfile can't open:"<< json_path.string();
		mx.unlock();
		return 0;
	}
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(in, root))
	{
		in.close();
		BOOST_LOG_TRIVIAL(info) << "jsonfile parse error";
		mx.unlock();
		return 0;
	}
	in.close();
	//BOOST_LOG_TRIVIAL(debug) << root.toStyledString();
	plusdef.json_value = root;
	plusdef.plus_file_name = path.filename().string();
	plusdef.plus_path = parent_path;
	try
	{
		plusdef.ret = root["ret"].asInt();
		plusdef.apiver = root["apiver"].asInt();
		plusdef.appid = root["appid"].asString();
		plusdef.name = root["name"].asString();
		plusdef.version = root["version"].asString();
		plusdef.version_id = root["version_id"].asInt();
		plusdef.author = root["author"].asString();
		plusdef.description = root["description"].asString();
		plusdef.is_enable = false;
	}catch(const std::exception & e)
	{
		BOOST_LOG_TRIVIAL(info) << e.what();
		mx.unlock();
		return 0;
	}
	

	Json::Value event_arr =  root["event"];

	if(event_arr.isNull())
	{	
		Json::Value v;
		v.resize(0);
		event_arr = v;  
	}

	if(!event_arr.isArray())
	{
		BOOST_LOG_TRIVIAL(info) << "bad jsonfile's event";
		mx.unlock();
		return 0;
	}
	for(size_t i = 0;i < event_arr.size();++i)
	{
		Event event;
		try
		{
			event.name = event_arr[i]["name"].asString();
			event.id = event_arr[i]["id"].asInt();
			event.priority = event_arr[i]["priority"].asInt();
			event.type = event_arr[i]["type"].asInt();
			event.function_name = event_arr[i]["function"].asString();
		}catch(const std::exception & e)
		{
			BOOST_LOG_TRIVIAL(info) << e.what();
			mx.unlock();
			return 0;
		}
		
		event.function_ptr = NULL;
		plusdef.event.push_back(event);
	}

	
	Json::Value auth_arr =  root["auth"];

	if(auth_arr.isNull())
	{	
		Json::Value v;
		v.resize(0);
		auth_arr = v;  
	}

	if(!auth_arr.isArray())
	{
		BOOST_LOG_TRIVIAL(info) << "bad jsonfile's auth";
		mx.unlock();
		return 0;
	}

	for(size_t i = 0;i < auth_arr.size();++i)
	{
		try
		{
			plusdef.auth.push_back(auth_arr[i].asInt());
		}
		catch(const std::exception & e)
		{
			BOOST_LOG_TRIVIAL(info) << e.what();
			mx.unlock();
			return 0;
		}
	}



	HMODULE dll_ptr = LoadLibraryA(path.string().c_str());
	if(!dll_ptr)
	{
		BOOST_LOG_TRIVIAL(info) << "dll can't load:" << plusdef.plus_file_name;
		mx.unlock();
		return 0;
	}


	Json::Value menu_arr =  root["menu"];

	if(menu_arr.isNull())
	{	
		Json::Value v;
		v.resize(0);
		menu_arr = v;  
	}	

	if(!menu_arr.isArray())
	{
		BOOST_LOG_TRIVIAL(info) << "bad jsonfile's menu";
		mx.unlock();
		return 0;
	}

	for(size_t i = 0;i < menu_arr.size();++i)
	{
		Menu menu;
		try
		{
			menu.function_name = menu_arr[i]["name"].asString();
			menu.name = menu_arr[i]["function"].asString();
			menu.function_ptr = (void *)GetProcAddress(dll_ptr,menu.name.c_str());
			if(menu.function_ptr == NULL)
			{
				BOOST_LOG_TRIVIAL(info) << "menu function not fonud";
				FreeLibrary(dll_ptr);
				mx.unlock();
				return 0;
			}
		}catch(const std::exception & e)
		{
			BOOST_LOG_TRIVIAL(info) << e.what();
			FreeLibrary(dll_ptr);
			mx.unlock();
			return 0;
		}
		plusdef.menu.push_back(menu);
	}

	for(size_t i = 0;i < plusdef.event.size();++i)
	{
		std::string function_name = plusdef.event[i].function_name;
		void * dllFunc = (void *)GetProcAddress(dll_ptr,function_name.c_str());
		if(dllFunc == NULL)
		{
			FreeLibrary(dll_ptr);
			BOOST_LOG_TRIVIAL(info) << "get function " << function_name  << " from " << plusdef.plus_file_name << " failed";
			mx.unlock();
			return 0;
		}
		plusdef.event[i].function_ptr = dllFunc;
	}


	//传递ac
	__int32 plus_ac = g_ac;
	g_ac++;
	typedef __int32(  __stdcall * fun_ptr_type_1)(__int32);
	typedef __int32(  __stdcall * fun_ptr_type_2)();
	fun_ptr_type_1 fun_ptr1 = (fun_ptr_type_1)GetProcAddress(dll_ptr,"Initialize");
	fun_ptr_type_2 fun_ptr2 = (fun_ptr_type_2)GetProcAddress(dll_ptr,"AppInfo");
	mx.unlock();
	
	if(!fun_ptr1)
	{
		FreeLibrary(dll_ptr);
		BOOST_LOG_TRIVIAL(info) << "get function " << "Initialize"  << " from " << plusdef.plus_file_name << " failed";
		return 0;
	}
	BOOST_LOG_TRIVIAL(debug) <<"call plus's fun Initialize: " << fun_ptr1;
	fun_ptr1(plus_ac);

	//AppInfo 在打包后不会被调用
	//if(!fun_ptr2)
	//{
	//	FreeLibrary(dll_ptr);
	//	BOOST_LOG_TRIVIAL(info) << "get function " << "AppInfo"  << " from " << plusdef.plus_file_name << " failed";
	//	return 0;
	//}
	//BOOST_LOG_TRIVIAL(debug) <<"call plus's fun AppInfo: " << fun_ptr2;
	//fun_ptr2();

	boost::recursive_mutex::scoped_lock lock(mx);
	plusdef.ac = plus_ac;

	plusdef.dll_ptr = dll_ptr;

	plus_map[plus_ac] = plusdef;
	BOOST_LOG_TRIVIAL(info) << "load " << plusdef.name << " success!";
	return plus_ac;
}

bool Plus::del_plus( __int32 ac )
{
	return false;
}

std::pair<bool,Plus::PlusDef> Plus::get_plusdef(__int32 ac)
{
	boost::recursive_mutex::scoped_lock lock(mx);
	BOOST_AUTO(const  & it,plus_map.find(ac));
	if(it == plus_map.end())
	{
		return std::pair<__int32,PlusDef>(false,PlusDef());
	}
	return std::pair<__int32,PlusDef>(true,it->second);
}

std::map<__int32,Plus::PlusDef> Plus::get_plus_map()
{
	boost::recursive_mutex::scoped_lock lock(mx);
	return plus_map;
}

bool Plus::enable_plus(__int32 ac)
{
	mx.lock();
	BOOST_AUTO(it,plus_map.find(ac));
	if(it == plus_map.end())
	{
		mx.unlock();
		return false;
	}
	if(it->second.is_enable)
	{
		mx.unlock();
		return true;
	}
	void * void_fun_ptr = get_plus_function(it->second,cq_event_enable_id);
	if(void_fun_ptr)
	{
		mx.unlock();
		BOOST_LOG_TRIVIAL(debug) << "call "<<it->second.name <<"'s fun:cq_event_enable: " <<void_fun_ptr;
		((cq_funtype(event_enable))void_fun_ptr)(); // 调用
		mx.lock();
	}
	it->second.is_enable = true;
	mx.unlock();
	return true;
}

bool Plus::disable_plus(__int32 ac)
{
	mx.lock();
	BOOST_AUTO(it,plus_map.find(ac));
	if(it == plus_map.end())
	{
		mx.unlock();
		return false;
	}
	if(it->second.is_enable == false)
	{
		mx.unlock();
		return true;
	}
	void * void_fun_ptr = get_plus_function(it->second,cq_event_disable_id);
	if(void_fun_ptr)
	{
		mx.unlock();
		BOOST_LOG_TRIVIAL(debug) << "call plus's fun:cq_event_disable: " <<void_fun_ptr;
		((cq_event_disable_funtype)void_fun_ptr)(); // 调用
		mx.lock();
	}
	mx.unlock();
	return true;
}

void * Plus::get_plus_function(const PlusDef & plus_def,__int32 id)
{
	const std::vector<Event> & event_vec =  plus_def.event;
	for(size_t i = 0;i < event_vec.size();++i)
	{
		if(event_vec[i].type == id)
		{
			return event_vec[i].function_ptr;
		}
	}
	return NULL;
}
