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
    if (
        code == 0x0023 ||
        code == 0x002A ||
        (code >= 0x0030 && code <= 0x0039) ||
        code == 0x00A9 ||
        code == 0x00AE ||
        code == 0x203C ||
        code == 0x2049 ||
        code == 0x2122 ||
        code == 0x2139 ||
        code == 0x2194 ||
        code == 0x2195 ||
        (code >= 0x21A9 && code <= 0x21AA) ||
        (code >= 0x231A && code <= 0x231B) ||
        code == 0x2328 ||
        code == 0x23CF ||
        (code >= 0x23E9 && code <= 0x23EC) ||
        (code >= 0x23ED && code <= 0x23EE) ||
        code == 0x23EF ||
        code == 0x23F0 ||
        (code >= 0x23F1 && code <= 0x23F2) ||
        code == 0x23F3 ||
        (code >= 0x23F8 && code <= 0x23FA) ||
        code == 0x24C2 ||
        (code >= 0x25AA && code <= 0x25AB) ||
        code == 0x25B6 ||
        code == 0x25C0 ||
        (code >= 0x25FB && code <= 0x25FE) ||
        (code >= 0x2600 && code <= 0x2601) ||
        (code >= 0x2602 && code <= 0x2603) ||
        code == 0x2604 ||
        code == 0x260E ||
        code == 0x2611 ||
        (code >= 0x2614 && code <= 0x2615) ||
        code == 0x2618 ||
        code == 0x261D ||
        code == 0x2620 ||
        (code >= 0x2622 && code <= 0x2623) ||
        code == 0x2626 ||
        code == 0x262A ||
        code == 0x262E ||
        code == 0x262F ||
        (code >= 0x2638 && code <= 0x2639) ||
        code == 0x263A ||
        code == 0x2640 ||
        code == 0x2642 ||
        (code >= 0x2648 && code <= 0x2653) ||
        code == 0x265F ||
        code == 0x2660 ||
        code == 0x2663 ||
        (code >= 0x2665 && code <= 0x2666) ||
        code == 0x2668 ||
        code == 0x267B ||
        code == 0x267E ||
        code == 0x267F ||
        code == 0x2692 ||
        code == 0x2693 ||
        code == 0x2694 ||
        code == 0x2695 ||
        (code >= 0x2696 && code <= 0x2697) ||
        code == 0x2699 ||
        (code >= 0x269B && code <= 0x269C) ||
        (code >= 0x26A0 && code <= 0x26A1) ||
        code == 0x26A7 ||
        (code >= 0x26AA && code <= 0x26AB) ||
        (code >= 0x26B0 && code <= 0x26B1) ||
        (code >= 0x26BD && code <= 0x26BE) ||
        (code >= 0x26C4 && code <= 0x26C5) ||
        code == 0x26C8 ||
        code == 0x26CE ||
        code == 0x26CF ||
        code == 0x26D1 ||
        code == 0x26D3 ||
        code == 0x26D4 ||
        code == 0x26E9 ||
        code == 0x26EA ||
        (code >= 0x26F0 && code <= 0x26F1) ||
        (code >= 0x26F2 && code <= 0x26F3) ||
        code == 0x26F4 ||
        code == 0x26F5 ||
        (code >= 0x26F7 && code <= 0x26F9) ||
        code == 0x26FA ||
        code == 0x26FD ||
        code == 0x2702 ||
        code == 0x2705 ||
        (code >= 0x2708 && code <= 0x270C) ||
        code == 0x270D ||
        code == 0x270F ||
        code == 0x2712 ||
        code == 0x2714 ||
        code == 0x2716 ||
        code == 0x271D ||
        code == 0x2721 ||
        code == 0x2728 ||
        (code >= 0x2733 && code <= 0x2734) ||
        code == 0x2744 ||
        code == 0x2747 ||
        code == 0x274C ||
        code == 0x274E ||
        (code >= 0x2753 && code <= 0x2755) ||
        code == 0x2757 ||
        code == 0x2763 ||
        code == 0x2764 ||
        (code >= 0x2795 && code <= 0x2797) ||
        code == 0x27A1 ||
        code == 0x27B0 ||
        code == 0x27BF ||
        (code >= 0x2934 && code <= 0x2935) ||
        (code >= 0x2B05 && code <= 0x2B07) ||
        (code >= 0x2B1B && code <= 0x2B1C) ||
        code == 0x2B50 ||
        code == 0x2B55 ||
        code == 0x3030 ||
        code == 0x303D ||
        code == 0x3297 ||
        code == 0x3299 ||
        code == 0x1F004 ||
        code == 0x1F0CF ||
        (code >= 0x1F170 && code <= 0x1F171) ||
        (code >= 0x1F17E && code <= 0x1F17F) ||
        code == 0x1F18E ||
        (code >= 0x1F191 && code <= 0x1F19A) ||
        (code >= 0x1F1E6 && code <= 0x1F1FF) ||
        (code >= 0x1F201 && code <= 0x1F202) ||
        code == 0x1F21A ||
        code == 0x1F22F ||
        (code >= 0x1F232 && code <= 0x1F23A) ||
        (code >= 0x1F250 && code <= 0x1F251) ||
        (code >= 0x1F300 && code <= 0x1F30C) ||
        (code >= 0x1F30D && code <= 0x1F30E) ||
        code == 0x1F30F ||
        code == 0x1F310 ||
        code == 0x1F311 ||
        code == 0x1F312 ||
        (code >= 0x1F313 && code <= 0x1F315) ||
        (code >= 0x1F316 && code <= 0x1F318) ||
        code == 0x1F319 ||
        code == 0x1F31A ||
        code == 0x1F31B ||
        code == 0x1F31C ||
        (code >= 0x1F31D && code <= 0x1F31E) ||
        (code >= 0x1F31F && code <= 0x1F320) ||
        code == 0x1F321 ||
        (code >= 0x1F324 && code <= 0x1F32C) ||
        (code >= 0x1F32D && code <= 0x1F32F) ||
        (code >= 0x1F330 && code <= 0x1F331) ||
        (code >= 0x1F332 && code <= 0x1F333) ||
        (code >= 0x1F334 && code <= 0x1F335) ||
        code == 0x1F336 ||
        (code >= 0x1F337 && code <= 0x1F34A) ||
        code == 0x1F34B ||
        (code >= 0x1F34C && code <= 0x1F34F) ||
        code == 0x1F350 ||
        (code >= 0x1F351 && code <= 0x1F37B) ||
        code == 0x1F37C ||
        code == 0x1F37D ||
        (code >= 0x1F37E && code <= 0x1F37F) ||
        (code >= 0x1F380 && code <= 0x1F393) ||
        (code >= 0x1F396 && code <= 0x1F397) ||
        (code >= 0x1F399 && code <= 0x1F39B) ||
        (code >= 0x1F39E && code <= 0x1F39F) ||
        (code >= 0x1F3A0 && code <= 0x1F3C4) ||
        code == 0x1F3C5 ||
        code == 0x1F3C6 ||
        code == 0x1F3C7 ||
        code == 0x1F3C8 ||
        code == 0x1F3C9 ||
        code == 0x1F3CA ||
        (code >= 0x1F3CB && code <= 0x1F3CE) ||
        (code >= 0x1F3CF && code <= 0x1F3D3) ||
        (code >= 0x1F3D4 && code <= 0x1F3DF) ||
        (code >= 0x1F3E0 && code <= 0x1F3E3) ||
        code == 0x1F3E4 ||
        (code >= 0x1F3E5 && code <= 0x1F3F0) ||
        code == 0x1F3F3 ||
        code == 0x1F3F4 ||
        code == 0x1F3F5 ||
        code == 0x1F3F7 ||
        (code >= 0x1F3F8 && code <= 0x1F407) ||
        code == 0x1F408 ||
        (code >= 0x1F409 && code <= 0x1F40B) ||
        (code >= 0x1F40C && code <= 0x1F40E) ||
        (code >= 0x1F40F && code <= 0x1F410) ||
        (code >= 0x1F411 && code <= 0x1F412) ||
        code == 0x1F413 ||
        code == 0x1F414 ||
        code == 0x1F415 ||
        code == 0x1F416 ||
        (code >= 0x1F417 && code <= 0x1F429) ||
        code == 0x1F42A ||
        (code >= 0x1F42B && code <= 0x1F43E) ||
        code == 0x1F43F ||
        code == 0x1F440 ||
        code == 0x1F441 ||
        (code >= 0x1F442 && code <= 0x1F464) ||
        code == 0x1F465 ||
        (code >= 0x1F466 && code <= 0x1F46B) ||
        (code >= 0x1F46C && code <= 0x1F46D) ||
        (code >= 0x1F46E && code <= 0x1F4AC) ||
        code == 0x1F4AD ||
        (code >= 0x1F4AE && code <= 0x1F4B5) ||
        (code >= 0x1F4B6 && code <= 0x1F4B7) ||
        (code >= 0x1F4B8 && code <= 0x1F4EB) ||
        (code >= 0x1F4EC && code <= 0x1F4ED) ||
        code == 0x1F4EE ||
        code == 0x1F4EF ||
        (code >= 0x1F4F0 && code <= 0x1F4F4) ||
        code == 0x1F4F5 ||
        (code >= 0x1F4F6 && code <= 0x1F4F7) ||
        code == 0x1F4F8 ||
        (code >= 0x1F4F9 && code <= 0x1F4FC) ||
        code == 0x1F4FD ||
        (code >= 0x1F4FF && code <= 0x1F502) ||
        code == 0x1F503 ||
        (code >= 0x1F504 && code <= 0x1F507) ||
        code == 0x1F508 ||
        code == 0x1F509 ||
        (code >= 0x1F50A && code <= 0x1F514) ||
        code == 0x1F515 ||
        (code >= 0x1F516 && code <= 0x1F52B) ||
        (code >= 0x1F52C && code <= 0x1F52D) ||
        (code >= 0x1F52E && code <= 0x1F53D) ||
        (code >= 0x1F549 && code <= 0x1F54A) ||
        (code >= 0x1F54B && code <= 0x1F54E) ||
        (code >= 0x1F550 && code <= 0x1F55B) ||
        (code >= 0x1F55C && code <= 0x1F567) ||
        (code >= 0x1F56F && code <= 0x1F570) ||
        (code >= 0x1F573 && code <= 0x1F579) ||
        code == 0x1F57A ||
        code == 0x1F587 ||
        (code >= 0x1F58A && code <= 0x1F58D) ||
        code == 0x1F590 ||
        (code >= 0x1F595 && code <= 0x1F596) ||
        code == 0x1F5A4 ||
        code == 0x1F5A5 ||
        code == 0x1F5A8 ||
        (code >= 0x1F5B1 && code <= 0x1F5B2) ||
        code == 0x1F5BC ||
        (code >= 0x1F5C2 && code <= 0x1F5C4) ||
        (code >= 0x1F5D1 && code <= 0x1F5D3) ||
        (code >= 0x1F5DC && code <= 0x1F5DE) ||
        code == 0x1F5E1 ||
        code == 0x1F5E3 ||
        code == 0x1F5E8 ||
        code == 0x1F5EF ||
        code == 0x1F5F3 ||
        code == 0x1F5FA ||
        (code >= 0x1F5FB && code <= 0x1F5FF) ||
        code == 0x1F600 ||
        (code >= 0x1F601 && code <= 0x1F606) ||
        (code >= 0x1F607 && code <= 0x1F608) ||
        (code >= 0x1F609 && code <= 0x1F60D) ||
        code == 0x1F60E ||
        code == 0x1F60F ||
        code == 0x1F610 ||
        code == 0x1F611 ||
        (code >= 0x1F612 && code <= 0x1F614) ||
        code == 0x1F615 ||
        code == 0x1F616 ||
        code == 0x1F617 ||
        code == 0x1F618 ||
        code == 0x1F619 ||
        code == 0x1F61A ||
        code == 0x1F61B ||
        (code >= 0x1F61C && code <= 0x1F61E) ||
        code == 0x1F61F ||
        (code >= 0x1F620 && code <= 0x1F625) ||
        (code >= 0x1F626 && code <= 0x1F627) ||
        (code >= 0x1F628 && code <= 0x1F62B) ||
        code == 0x1F62C ||
        code == 0x1F62D ||
        (code >= 0x1F62E && code <= 0x1F62F) ||
        (code >= 0x1F630 && code <= 0x1F633) ||
        code == 0x1F634 ||
        code == 0x1F635 ||
        code == 0x1F636 ||
        (code >= 0x1F637 && code <= 0x1F640) ||
        (code >= 0x1F641 && code <= 0x1F644) ||
        (code >= 0x1F645 && code <= 0x1F64F) ||
        code == 0x1F680 ||
        (code >= 0x1F681 && code <= 0x1F682) ||
        (code >= 0x1F683 && code <= 0x1F685) ||
        code == 0x1F686 ||
        code == 0x1F687 ||
        code == 0x1F688 ||
        code == 0x1F689 ||
        (code >= 0x1F68A && code <= 0x1F68B) ||
        code == 0x1F68C ||
        code == 0x1F68D ||
        code == 0x1F68E ||
        code == 0x1F68F ||
        code == 0x1F690 ||
        (code >= 0x1F691 && code <= 0x1F693) ||
        code == 0x1F694 ||
        code == 0x1F695 ||
        code == 0x1F696 ||
        code == 0x1F697 ||
        code == 0x1F698 ||
        (code >= 0x1F699 && code <= 0x1F69A) ||
        (code >= 0x1F69B && code <= 0x1F6A1) ||
        code == 0x1F6A2 ||
        code == 0x1F6A3 ||
        (code >= 0x1F6A4 && code <= 0x1F6A5) ||
        code == 0x1F6A6 ||
        (code >= 0x1F6A7 && code <= 0x1F6AD) ||
        (code >= 0x1F6AE && code <= 0x1F6B1) ||
        code == 0x1F6B2 ||
        (code >= 0x1F6B3 && code <= 0x1F6B5) ||
        code == 0x1F6B6 ||
        (code >= 0x1F6B7 && code <= 0x1F6B8) ||
        (code >= 0x1F6B9 && code <= 0x1F6BE) ||
        code == 0x1F6BF ||
        code == 0x1F6C0 ||
        (code >= 0x1F6C1 && code <= 0x1F6C5) ||
        code == 0x1F6CB ||
        code == 0x1F6CC ||
        (code >= 0x1F6CD && code <= 0x1F6CF) ||
        code == 0x1F6D0 ||
        (code >= 0x1F6D1 && code <= 0x1F6D2) ||
        code == 0x1F6D5 ||
        (code >= 0x1F6D6 && code <= 0x1F6D7) ||
        (code >= 0x1F6DD && code <= 0x1F6DF) ||
        (code >= 0x1F6E0 && code <= 0x1F6E5) ||
        code == 0x1F6E9 ||
        (code >= 0x1F6EB && code <= 0x1F6EC) ||
        code == 0x1F6F0 ||
        code == 0x1F6F3 ||
        (code >= 0x1F6F4 && code <= 0x1F6F6) ||
        (code >= 0x1F6F7 && code <= 0x1F6F8) ||
        code == 0x1F6F9 ||
        code == 0x1F6FA ||
        (code >= 0x1F6FB && code <= 0x1F6FC) ||
        (code >= 0x1F7E0 && code <= 0x1F7EB) ||
        code == 0x1F7F0 ||
        code == 0x1F90C ||
        (code >= 0x1F90D && code <= 0x1F90F) ||
        (code >= 0x1F910 && code <= 0x1F918) ||
        (code >= 0x1F919 && code <= 0x1F91E) ||
        code == 0x1F91F ||
        (code >= 0x1F920 && code <= 0x1F927) ||
        (code >= 0x1F928 && code <= 0x1F92F) ||
        code == 0x1F930 ||
        (code >= 0x1F931 && code <= 0x1F932) ||
        (code >= 0x1F933 && code <= 0x1F93A) ||
        (code >= 0x1F93C && code <= 0x1F93E) ||
        code == 0x1F93F ||
        (code >= 0x1F940 && code <= 0x1F945) ||
        (code >= 0x1F947 && code <= 0x1F94B) ||
        code == 0x1F94C ||
        (code >= 0x1F94D && code <= 0x1F94F) ||
        (code >= 0x1F950 && code <= 0x1F95E) ||
        (code >= 0x1F95F && code <= 0x1F96B) ||
        (code >= 0x1F96C && code <= 0x1F970) ||
        code == 0x1F971 ||
        code == 0x1F972 ||
        (code >= 0x1F973 && code <= 0x1F976) ||
        (code >= 0x1F977 && code <= 0x1F978) ||
        code == 0x1F979 ||
        code == 0x1F97A ||
        code == 0x1F97B ||
        (code >= 0x1F97C && code <= 0x1F97F) ||
        (code >= 0x1F980 && code <= 0x1F984) ||
        (code >= 0x1F985 && code <= 0x1F991) ||
        (code >= 0x1F992 && code <= 0x1F997) ||
        (code >= 0x1F998 && code <= 0x1F9A2) ||
        (code >= 0x1F9A3 && code <= 0x1F9A4) ||
        (code >= 0x1F9A5 && code <= 0x1F9AA) ||
        (code >= 0x1F9AB && code <= 0x1F9AD) ||
        (code >= 0x1F9AE && code <= 0x1F9AF) ||
        (code >= 0x1F9B0 && code <= 0x1F9B9) ||
        (code >= 0x1F9BA && code <= 0x1F9BF) ||
        code == 0x1F9C0 ||
        (code >= 0x1F9C1 && code <= 0x1F9C2) ||
        (code >= 0x1F9C3 && code <= 0x1F9CA) ||
        code == 0x1F9CB ||
        code == 0x1F9CC ||
        (code >= 0x1F9CD && code <= 0x1F9CF) ||
        (code >= 0x1F9D0 && code <= 0x1F9E6) ||
        (code >= 0x1F9E7 && code <= 0x1F9FF) ||
        (code >= 0x1FA70 && code <= 0x1FA73) ||
        code == 0x1FA74 ||
        (code >= 0x1FA78 && code <= 0x1FA7A) ||
        (code >= 0x1FA7B && code <= 0x1FA7C) ||
        (code >= 0x1FA80 && code <= 0x1FA82) ||
        (code >= 0x1FA83 && code <= 0x1FA86) ||
        (code >= 0x1FA90 && code <= 0x1FA95) ||
        (code >= 0x1FA96 && code <= 0x1FAA8) ||
        (code >= 0x1FAA9 && code <= 0x1FAAC) ||
        (code >= 0x1FAB0 && code <= 0x1FAB6) ||
        (code >= 0x1FAB7 && code <= 0x1FABA) ||
        (code >= 0x1FAC0 && code <= 0x1FAC2) ||
        (code >= 0x1FAC3 && code <= 0x1FAC5) ||
        (code >= 0x1FAD0 && code <= 0x1FAD6) ||
        (code >= 0x1FAD7 && code <= 0x1FAD9) ||
        (code >= 0x1FAE0 && code <= 0x1FAE7) ||
        (code >= 0x1FAF0 && code <= 0x1FAF6))
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
