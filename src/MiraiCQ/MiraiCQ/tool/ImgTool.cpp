#include "ImgTool.h"

#include <httplib/httplib.h>
#include <regex>

static ImgTool::ImgInfo parse_img(const std::string& body)
{
    ImgTool::ImgInfo ret_info;
    if (body.size() >= 26 &&  (unsigned char)body[0] == 0x42 && (unsigned char)body[1] == 0x4d)
    {
        //bmp img
        ret_info.type = "bmp";

        ret_info.width += (unsigned char)body[0x12];
        ret_info.width += (unsigned char)body[0x13] * (unsigned int)256;
        ret_info.width += (unsigned char)body[0x14] * (unsigned int)256 * 256;
        ret_info.width += (unsigned char)body[0x15] * (unsigned int)256 * 256 * 256;

        ret_info.height += (unsigned char)body[0x16];
        ret_info.height += (unsigned char)body[0x17] * (unsigned int)256;
        ret_info.height += (unsigned char)body[0x18] * (unsigned int)256 * 256;
        ret_info.height += (unsigned char)body[0x19] * (unsigned int)256 * 256 * 256;
         
    }
    else if (body.size() >= 24 && !memcmp(&body[12], "IHDR", 4) && !memcmp(&body[1], "PNG", 3))
    {
        ret_info.type = "png";
        ret_info.width += (unsigned char)body[0x13];
        ret_info.width += (unsigned char)body[0x12] * (unsigned int)256;
        ret_info.width += (unsigned char)body[0x11] * (unsigned int)256 * 256;
        ret_info.width += (unsigned char)body[0x10] * (unsigned int)256 * 256 * 256;

        ret_info.height += (unsigned char)body[0x17];
        ret_info.height += (unsigned char)body[0x16] * (unsigned int)256;
        ret_info.height += (unsigned char)body[0x15] * (unsigned int)256 * 256;
        ret_info.height += (unsigned char)body[0x14] * (unsigned int)256 * 256 * 256;
    }
    else if (body.size() >= 16 && !memcmp(&body[1], "PNG", 3))
    {
        ret_info.type = "png";
        ret_info.width += (unsigned char)body[11];
        ret_info.width += (unsigned char)body[10] * (unsigned int)256;
        ret_info.width += (unsigned char)body[9] * (unsigned int)256 * 256;
        ret_info.width += (unsigned char)body[8] * (unsigned int)256 * 256 * 256;

        ret_info.height += (unsigned char)body[15];
        ret_info.height += (unsigned char)body[14] * (unsigned int)256;
        ret_info.height += (unsigned char)body[13] * (unsigned int)256 * 256;
        ret_info.height += (unsigned char)body[12] * (unsigned int)256 * 256 * 256;
    }
    else if (body.size() >= 3 && (unsigned char)body[0] == 0xff && (unsigned char)body[1] == 0xd8)
    {
        ret_info.type = "jpg";
        size_t i = 2;
        unsigned char b = (unsigned char)body[i];
        while (b != 0xDA)
        {
            while (b != 0xFF)
            {
                ++i;
                if (i >= body.size())
                {
                    return ret_info;
                }
                b = (unsigned char)body[i];
            }
            while (b == 0xFF)
            {
                ++i;
                if (i >= body.size())
                {
                    return ret_info;
                }
                b = (unsigned char)body[i];
            }
            if (b >= 0xC0 && b <= 0xC3)
            {
                if (i + 8 > body.size())
                {
                    return ret_info;
                }
                ret_info.height = (unsigned char)body[i + 4] * (unsigned int)256 + (unsigned char)body[i + 5];
                ret_info.width = (unsigned char)body[i + 6] * (unsigned int)256 + (unsigned char)body[i + 7];
                return ret_info;
            }
            else
            {
                if (i + 2 >= body.size())
                {
                    return ret_info;
                }
                int off = (unsigned char)body[i + 1] * (unsigned int)256 + (unsigned char)body[i + 2] - 2;
                i += 2;
                if (i + off > body.size())
                {
                    return ret_info;
                }
                i += off;
            }
        }
    }
    else if(body.size() >= 10 && (!memcmp(&body[0], "GIF87a", 6) || !memcmp(&body[0], "GIF89a", 6)))
    {
        ret_info.type = "gif";
        ret_info.width = (unsigned char)body[6] + (unsigned char)body[7] * (unsigned int)256;
        ret_info.height = (unsigned char)body[8] + (unsigned char)body[9] * (unsigned int)256;
    }

    return ret_info;
}

bool ImgTool::get_info(const std::string& url, ImgInfo& info)
{
    using namespace httplib;
    std::string body;
    unsigned size = 0;
    std::string host;
    std::string get_url;
    try
    {
        std::regex r("((http://|https://)(.*?)(/.*))");
        std::smatch sm;
        if (std::regex_search(url, sm, r)) {
            if (sm.size() != 5)
            {
                return false;
            }
            host = "http://" + sm[3].str();
            get_url = sm[4].str();
        }
        else {
            return false;
        }
        httplib::Client cli(host);
        auto res = cli.Get(
            get_url.c_str(), Headers(),
            [&](const Response& response) {
            if (response.status != 200) 
            {
                return false;
            }
            std::string size_str = response.get_header_value("Content-Length");
            if (size_str != "")
            {
                size =  (unsigned int)atol(size_str.c_str());
            }
            return true;
        },
            [&](const char* data, size_t data_length) {
            body.append(data, data_length);
            info = parse_img(body);
            if (info.type == "")
            {
                /* ˵�����޷�������ͼƬ��ʽ */
                return false;
            }
            if (info.height == 0 || info.width == 0)
            {
                /* ˵���ܽ�����ֻ�Ƕ�ȡ�����ݻ������� */
                return true; /* ������ */
            }
            /* ˵�������ɹ��� */
            return false; 
        });
    }
    catch (const std::exception&)
    {
        /* �����쳣��ͨ����������ʴ��� */
        return false;
    }
    info.size = size;
    if (info.height == 0 || info.width == 0 || info.size == 0 || info.type == "")
    {
        /* û�л�ȡ������Ϣ��˵������ʧ�� */
        return false;
    }
	return  true;
}

bool ImgTool::download_img(const std::string& url, const std::string& save_path) noexcept
{
    using namespace httplib;
    std::string host;
    std::string get_url;
    try
    {
        std::regex r("((http://|https://)(.*?)(/.*))");
        std::smatch sm;
        if (std::regex_search(url, sm, r)) {
            if (sm.size() != 5)
            {
                return false;
            }
            host = "http://" + sm[3].str();
            get_url = sm[4].str();
        }
        else {
            return false;
        }
        httplib::Client cli(host);
        auto res = cli.Get(get_url.c_str());
        if (!res)
        {
            /* ����ִ��ʧ�� */
            return false;
        }
        if (res->status != 200)
        {
            /* ���緵��ʧ�� */
            return false;
        }
        std::ofstream out_file;
        out_file.open(save_path,std::ios::binary);
        if (!out_file.is_open())
        {
            /* �ļ���ʧ�� */
            return false;
        }
        /* д���ļ� */
        out_file.write(res->body.data(), res->body.size());
        /* �ر��ļ� */
        out_file.close();
    }
    catch (const std::exception&)
    {
        /* �����쳣��ͨ����������ʴ��� */
        return false;
    }
    return  true;
}