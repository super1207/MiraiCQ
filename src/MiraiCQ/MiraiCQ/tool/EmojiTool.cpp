#include "EmojiTool.h"

std::string u2utf8(int v)
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

int utf82u(const std::string& str)
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

int utf8_next_len(const std::string& str, int offset)
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

bool is_emoji(int code)
{
    if ((0x0080 <= code && code <= 0x02AF) ||
        (0x0300 <= code && code <= 0x03FF) ||
        (0x0600 <= code && code <= 0x06FF) ||
        (0x0C00 <= code && code <= 0x0C7F) ||
        (0x1DC0 <= code && code <= 0x1DFF) ||
        (0x1E00 <= code && code <= 0x1EFF) ||
        (0x2000 <= code && code <= 0x209F) ||
        (0x20D0 <= code && code <= 0x214F) ||
        (0x2190 <= code && code <= 0x23FF) ||
        (0x2460 <= code && code <= 0x25FF) ||
        (0x2600 <= code && code <= 0x27EF) ||
        (0x2900 <= code && code <= 0x29FF) ||
        (0x2B00 <= code && code <= 0x2BFF) ||
        (0x2C60 <= code && code <= 0x2C7F) ||
        (0x2E00 <= code && code <= 0x2E7F) ||
        (0xA490 <= code && code <= 0xA4CF) ||
        (0xE000 <= code && code <= 0xF8FF) ||
        (0xFE00 <= code && code <= 0xFE0F) ||
        (0xFE30 <= code && code <= 0xFE4F) ||
        (0x1F000 <= code && code <= 0x1F02F) ||
        (0x1F0A0 <= code && code <= 0x1F0FF) ||
        (0x1F100 <= code && code <= 0x1F64F) ||
        (0x1F680 <= code && code <= 0x1F6FF) ||
        (0x1F910 <= code && code <= 0x1F96B) ||
        (0x1F980 <= code && code <= 0x1F9E0))
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