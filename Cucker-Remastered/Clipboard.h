#pragma once
#include <Windows.h>
#include <iostream>
#include <string>

class Clipboard {
public:
    Clipboard() = default;

    // 将文本复制到剪贴板
    void copyWstring(const std::wstring& text) {
        if (!OpenClipboard(nullptr)) {
            return;
        }

        // 清空剪贴板
        EmptyClipboard();

        // 创建全局内存块，用于存储文本
        size_t len = (text.length() + 1) * sizeof(wchar_t); // 包括 null 终止符
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
        if (hMem == nullptr) {
            CloseClipboard();
            DWORD errorCode = GetLastError();
            throw std::runtime_error("Failed to allocate memory for clipboard. (" + std::to_string(errorCode) + ")");
        }

        // 锁定内存并复制数据
        wchar_t* pMem = static_cast<wchar_t*>(GlobalLock(hMem));
        wmemcpy(pMem, text.c_str(), text.length() + 1);
        GlobalUnlock(hMem);

        // 将内存数据设置为剪贴板内容
        if (SetClipboardData(CF_UNICODETEXT, hMem) == nullptr) {
            GlobalFree(hMem);
            CloseClipboard();
            throw std::runtime_error("Failed to set clipboard data.");
        }

        CloseClipboard();
    }

    // 从剪贴板粘贴文本
    std::wstring pasteWstring(size_t maxLength = 1000) {
        if (!OpenClipboard(nullptr)) {
            return L"";
        }

        if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) {
            CloseClipboard();
            return L"";
        }

        // 获取剪贴板数据
        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
        if (hData == nullptr) {
            CloseClipboard();
            return L"";
        }

        // 锁定数据并读取
        wchar_t* pMem = static_cast<wchar_t*>(GlobalLock(hData));
        std::wstring text(pMem);
        GlobalUnlock(hData);

        // 如果文本长度超过最大字数，则截断
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