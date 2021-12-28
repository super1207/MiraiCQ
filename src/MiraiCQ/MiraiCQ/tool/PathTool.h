#pragma once

#include <string>
#include <vector>

class PathTool
{
public:
	/*
	* 描述：判断目录是否存在
	* 参数`dir`：要判断的路径
	* 返回值：存在返回`true`,不存在或无法判断则返回`false`
	*/
	static bool is_dir_exist(const std::string & dir) ;

	/*
	* 描述：判断文件是否存在
	* 参数`file`：要判断的文件
	* 返回值：存在返回`true`,不存在或无法判断则返回`false`
	*/
	static bool is_file_exist(const std::string & file) ;

	/*
	* 描述：创建目录 (不可递归创建)
	* 参数`dir`：要创建的目录
	* 返回值：创建成功，或者目录已经存在返回`true`,创建失败返回`false`
	*/
	static bool create_dir(const std::string& dir) ;

	/*
	* 描述：删除目录
	* 参数`dir`：要删除的目录
	* 返回值：删除成功，或者目录原本就不存在存在返回`true`,删除失败返回`false`
	*/
	static bool del_dir(const std::string& dir) ;

	/*
	* 描述：删除文件
	* 参数`file`：要删除的文件
	* 返回值：删除成功，或者文件原本就不存在返回`true`,删除失败返回`false`
	*/
	static bool del_file(const std::string& file) ;

	/*
	* 描述：获得可执行文件（exe）所在的目录（绝对路径）
	* 返回值：返回exe所在目录(末尾带`'\'`)，失败则返回`""`
	*/
	static std::string get_exe_dir() ;

	/*
	* 描述：获得可执行文件（exe）的文件名
	* 返回值：返回exe文件名，失败则返回`""`
	*/
	static std::string get_exe_name() ;

	/*
	* 描述：获取目录下得所有文件
	* 参数`path`：目录
	* 返回值：文件名数组（只含文件名）
	*/
	static std::vector<std::string> get_path_file(const std::string & path) ;

	/*
	* 描述：重命名文件
	* 参数`old_name`：旧路径
	* 参数`new_name`：新路径
	* 返回值：成功返回`true`
	*/
	static bool rename(const std::string& old_name, const std::string& new_name) ;



};
