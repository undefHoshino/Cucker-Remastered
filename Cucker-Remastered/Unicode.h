#pragma once
#include <string>

inline int GetUnicodeWidth(unsigned char utf8char) {
	if (utf8char <= 31) return -1;

    // 如果开头是 0x80 - 0xBF，则表示该字节是多字节字符的剩余部分
    if (utf8char >= 0x80 && utf8char <= 0xBF) {
        return -2;
    }

    // 单字节字符（0x00 - 0x7F），占 1 个字符宽度
    if (utf8char <= 0x7F) {
        return 1;
    }

    // 双字节字符（0xC0 - 0xDF），占 2 个字符宽度
    if (utf8char <= 0xDF) {
        return 2;
    }

    // 三字节字符（0xE0 - 0xEF），通常是中文，占 3 个字符宽度
    if (utf8char <= 0xEF) {
        return 3;
    }

    // 四字节字符（0xF0 - 0xF7），通常是较少见的字符，占 4 个字符宽度
    if (utf8char <= 0xF7) {
        return 4;
    }

    return -1;
}