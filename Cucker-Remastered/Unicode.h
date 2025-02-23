#pragma once
#include <string>

inline int GetUnicodeWidth(unsigned char utf8char) {
	if (utf8char <= 31) return -1;

    // �����ͷ�� 0x80 - 0xBF�����ʾ���ֽ��Ƕ��ֽ��ַ���ʣ�ಿ��
    if (utf8char >= 0x80 && utf8char <= 0xBF) {
        return -2;
    }

    // ���ֽ��ַ���0x00 - 0x7F����ռ 1 ���ַ����
    if (utf8char <= 0x7F) {
        return 1;
    }

    // ˫�ֽ��ַ���0xC0 - 0xDF����ռ 2 ���ַ����
    if (utf8char <= 0xDF) {
        return 2;
    }

    // ���ֽ��ַ���0xE0 - 0xEF����ͨ�������ģ�ռ 3 ���ַ����
    if (utf8char <= 0xEF) {
        return 3;
    }

    // ���ֽ��ַ���0xF0 - 0xF7����ͨ���ǽ��ټ����ַ���ռ 4 ���ַ����
    if (utf8char <= 0xF7) {
        return 4;
    }

    return -1;
}