#include "EmojiTool.h"

static std::string u2utf8(int v)
{
    if (v < 0x80)
    {
        char t[2];
        t[0] = v;
        t[1] = 0;
        return t;
    }
    else if (v < 0x800)
    {
        char t[3];
        t[0] = 0xC0 | v >> 6;
        t[1] = 0x80 | v & 0x3F;
        t[2] = 0;
        return t;
    }
    else if (v < 0x10000)
    {
        char t[4];
        t[0] = 0xE0 | v >> 12;
        t[1] = 0x80 | v >> 6 & 0x3F;
        t[2] = 0x80 | v & 0x3F;
        t[3] = 0;
        return t;
    }
    else if (v < 0x200000)
    {
        char t[5];
        t[0] = 0xF0 | v >> 18;
        t[1] = 0x80 | v >> 12 & 0x3F;
        t[2] = 0x80 | v >> 6 & 0x3F;
        t[3] = 0x80 | v & 0x3F;
        t[4] = 0;
        return t;
    }
    else if (v < 0x1000000)
    {
        char t[6];
        t[0] = 0xF8 | v >> 24;
        t[1] = 0x80 | v >> 18 & 0x3F;
        t[2] = 0x80 | v >> 12 & 0x3F;
        t[3] = 0x80 | v >> 6 & 0x3F;
        t[4] = 0x80 | v & 0x3F;
        t[5] = 0;
        return t;
    }
    else if (v < 0x80000000)
    {
        char t[7];
        t[0] = 0xFC | v >> 30;
        t[1] = 0x80 | v >> 24 & 0x3F;
        t[2] = 0x80 | v >> 18 & 0x3F;
        t[3] = 0x80 | v >> 12 & 0x3F;
        t[4] = 0x80 | v >> 6 & 0x3F;
        t[5] = 0x80 | v & 0x3F;
        t[6] = 0;
        return t;
    }
    else
        return "\0";
}

static int utf82u(const std::string& str)
{
    int t = 0;
    switch (str.size())
    {
    case 1:
        t = (int)str[0];
        break;
    case 2:
        t = (int)(str[0] & 0x1F) << 6;
        t = t | (int)(str[1] & 0x3F);
        break;
    case 3:
        t = (int)(str[0] & 0x0F) << 12;
        t = t | (int)(str[1] & 0x3F) << 6;
        t = t | (int)(str[2] & 0x3F);
        break;
    case 4:
        t = (int)(str[0] & 0x07) << 18;
        t = t | (int)(str[1] & 0x3F) << 12;
        t = t | (int)(str[2] & 0x3F) << 6;
        t = t | (int)(str[3] & 0x3F);
        break;
    case 5:
        t = (int)(str[0] & 0x03) << 24;
        t = t | (int)(str[1] & 0x3F) << 18;
        t = t | (int)(str[2] & 0x3F) << 12;
        t = t | (int)(str[3] & 0x3F) << 6;
        t = t | (int)(str[4] & 0x3F);
        break;
    case 6:
        t = (int)(str[0] & 0x03) << 30;
        t = t | (int)(str[1] & 0x3F) << 24;
        t = t | (int)(str[2] & 0x3F) << 18;
        t = t | (int)(str[3] & 0x3F) << 12;
        t = t | (int)(str[4] & 0x3F) << 6;
        t = t | (int)(str[5] & 0x3F);
        break;
    default:
        return 0;
    }
    return t;
}

static int utf8_next_len(const std::string& str, int offset)
{
    if (str.size() <= (size_t)offset || str[offset] == 0)
        return 0;
    unsigned char c = (unsigned char)str[offset];
    if (c >= 0xFC)
        return 6;
    else if (c >= 0xF8)
        return 5;
    else if (c >= 0xF0)
        return 4;
    else if (c >= 0xE0)
        return 3;
    else if (c >= 0xC0)
        return 2;
    else if (c > 0x00)
        return 1;
    else
        return 0;
    return 0;
}

static bool is_emoji(int code)
{
    if ((code >= 0x2600 && code <= 0x27BF) || // 杂项符号与符号字体
        code == 0x303D || 
        code == 0x2049 || 
        code == 0x203C || 
        (code >= 0x2000 && code <= 0x200F) || //
        (code >= 0x2028 && code <= 0x202F) || //
        code == 0x205F || //
        (code >= 0x2065 && code <= 0x206F) || //
                /* 标点符号占用区域 */
        (code >= 0x2100 && code <= 0x214F) || // 字母符号
        (code >= 0x2300 && code <= 0x23FF) || // 各种技术符号
        (code >= 0x2B00 && code <= 0x2BFF) || // 箭头A
        (code >= 0x2900 && code <= 0x297F) || // 箭头B
        (code >= 0x3200 && code <= 0x32FF)) // 中文符号
        return true;
    return false;
}

std::string EmojiTool::escape_cq_emoji(const std::string& str)
{
    std::string ret;
    int start = 0;
    int code = 0;
    for (int len = utf8_next_len(str, start); len != 0; len = utf8_next_len(str, start))
    {
        std::string temp = str.substr(start, len);
        start += len;
        code = utf82u(temp);
        if (!is_emoji(code))
        {
            ret.append(temp);
            continue;
        }
        ret.append("[CQ:emoji,id=");
        ret.append(std::to_string(code));
        ret.append("]");
    }
    return ret;
}

std::string EmojiTool::unescape_cq_emoji(const std::string& str)
{
    int offset = 0;
    int id = 0;
    int s = 0, e = 0;
    std::string ret;
    while (true)
    {
        if ((s = str.find("[CQ:emoji,id=", offset)) == std::string::npos ||
            (e = str.find("]", s + 13)) == std::string::npos)
        {
            if (offset == 0)
                return str;
            ret.append(str.substr(offset, str.size() - offset));
            break;
        }
        try
        {
            id = std::stoi(str.substr(s + 13, e - s + 13));
        }
        catch (...)
        {
            return str;
        }
        ret.append(str.substr(offset, s - offset));
        ret.append(u2utf8(id));
        offset = e + 1;
    }
    return ret;
}

EmojiTool::EmojiTool()
{
}
