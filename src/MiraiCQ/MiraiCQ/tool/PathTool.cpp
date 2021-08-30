#include "PathTool.h"

#include <filesystem>
#include <windows.h>

using namespace std;
namespace  fs = std::filesystem;

bool PathTool::is_dir_exist(const string& dir) 
{
	try
	{
		return fs::is_directory(dir);
	}
	catch (const exception&)
	{
		return false;
	}
}

bool PathTool::is_file_exist(const string& file) 
{
	try
	{
		return fs::is_regular_file(file);
	}
	catch (const exception&)
	{
		return false;
	}
}

bool PathTool::create_dir(const string& dir) 
{
	try
	{
		/* 目录存在，直接返回`true` */
		if (is_dir_exist(dir))
		{
			return true;
		}
		fs::create_directory(dir);
		return is_dir_exist(dir);
	}
	catch (const exception&)
	{
		return false;
	}
}

bool PathTool::del_dir(const string& dir) 
{
	try
	{
		/* 目录不存在，直接返回`true` */
		if (!is_dir_exist(dir))
		{
			return true;
		}
		fs::remove_all(dir);
		return !is_dir_exist(dir);
	}
	catch (const exception&)
	{
		return false;
	}
	
}

bool PathTool::del_file(const std::string& file) 
{
	try
	{
		/* 文件不存在，直接返回`true` */
		if (!is_file_exist(file))
		{
			return true;
		}
		fs::remove(file);
		return !is_file_exist(file);
	}
	catch (const exception&)
	{
		return false;
	}
}

string PathTool::get_exe_dir() 
{
	string ret_str;
	char path_str[MAX_PATH + 1] = { 0 };
	const DWORD n = GetModuleFileNameA(NULL, path_str, MAX_PATH);
	if (n == 0)
	{
		ret_str ="";
	}
	else
	{
		*(strrchr(path_str, '\\')) = '\0';
		ret_str = path_str + string("\\");
	}
	return ret_str;
}

string PathTool::get_exe_name() 
{
	string ret_str;
	char path_str[MAX_PATH + 1] = { 0 };
	const DWORD n = GetModuleFileNameA(NULL, path_str, MAX_PATH);
	if (n == 0)
	{
		ret_str = "";
	}
	else
	{
		const char * pos = (strrchr(path_str, '\\')) + 1;
		ret_str = pos;
	}
	return ret_str;
}

std::vector<std::string> PathTool::get_path_file(const std::string& path) 
{
	vector<string> ret_vec;
	try
	{
		for (auto& p : fs::directory_iterator(path))
		{
			if (p.is_regular_file())
			{
				ret_vec.push_back(p.path().filename().string());
			}
		}
	}
	catch (const std::exception&)
	{

	}	
	return ret_vec;
}

bool PathTool::rename(const std::string& old_name, const std::string& new_name) 
{
	try
	{
		fs::rename(fs::path(old_name), fs::path(new_name));
	}
	catch (const std::exception&)
	{
		return false;
	}
	return true;
}
