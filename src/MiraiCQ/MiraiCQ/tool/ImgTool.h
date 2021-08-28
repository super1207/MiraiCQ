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
	};
	static bool get_info(const std::string& url, ImgInfo & info);
	static bool download_img(const std::string& url, const std::string& save_path) noexcept;
	

private:

};

