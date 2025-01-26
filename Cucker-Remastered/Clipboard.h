#pragma once
#include <Windows.h>
#include <iostream>
#include <string>

class Clipboard {
public:
    Clipboard() = default;

    // ���ı����Ƶ�������
    void copyWstring(const std::wstring& text) {
        if (!OpenClipboard(nullptr)) {
            return;
        }

        // ��ռ�����
        EmptyClipboard();

        // ����ȫ���ڴ�飬���ڴ洢�ı�
        size_t len = (text.length() + 1) * sizeof(wchar_t); // ���� null ��ֹ��
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
        if (hMem == nullptr) {
            CloseClipboard();
            DWORD errorCode = GetLastError();
            throw std::runtime_error("Failed to allocate memory for clipboard. (" + std::to_string(errorCode) + ")");
        }

        // �����ڴ沢��������
        wchar_t* pMem = static_cast<wchar_t*>(GlobalLock(hMem));
        wmemcpy(pMem, text.c_str(), text.length() + 1);
        GlobalUnlock(hMem);

        // ���ڴ���������Ϊ����������
        if (SetClipboardData(CF_UNICODETEXT, hMem) == nullptr) {
            GlobalFree(hMem);
            CloseClipboard();
            throw std::runtime_error("Failed to set clipboard data.");
        }

        CloseClipboard();
    }

    // �Ӽ�����ճ���ı�
    std::wstring pasteWstring(size_t maxLength = 1000) {
        if (!OpenClipboard(nullptr)) {
            return L"";
        }

        if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) {
            CloseClipboard();
            return L"";
        }

        // ��ȡ����������
        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
        if (hData == nullptr) {
            CloseClipboard();
            return L"";
        }

        // �������ݲ���ȡ
        wchar_t* pMem = static_cast<wchar_t*>(GlobalLock(hData));
        std::wstring text(pMem);
        GlobalUnlock(hData);

        // ����ı����ȳ��������������ض�
        if (text.length() > maxLength) {
            text = text.substr(0, maxLength);
        }

        CloseClipboard();

        return text;
    }
    bool isClipboardTextAvailable() {
        return IsClipboardFormatAvailable(CF_UNICODETEXT);
    }
};