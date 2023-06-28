#pragma once

#include <string>

class ImgTool
{
public:
	struct ImgInfo
	{
		unsigned int width = 0;
		unsigned int height = 0;
		unsigned int size = 0;
		std::string type = "";
		std::string md5_str = "";
	};
	/*
	* 描述：获得图片信息
	* 参数`url`：图片的url
	* 参数`info`：输出图片信息
	* 返回值：成功返回`true`，失败返回`false`
	*/
	static bool get_info(const std::string& url, ImgInfo & info, bool isqq);

	/*
	* 描述：下载图片
	* 参数`url`：图片的url
	* 参数`save_path`：图片保存路径
	* 返回值：成功返回`true`，失败返回`false`
	*/
	static bool download_img(const std::string& url, const std::string& save_path) ;

	/*
	* 描述：获得图片信息
	* 参数`body`：图片的body(可以只是一部分图片)
	* 返回值：图片信息(可能不完整)
	*/
	static ImgInfo parse_img(const std::string& body);
	

private:

};

