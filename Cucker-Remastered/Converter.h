#pragma once
#include <string>
#include <sstream>
#include "ConsoleSetting.h"

class Converter {
public:
    template<class Type, class Result>
    static Result Convert(Type text);

    // wstring 转换为 string
    template<>
    static std::string Convert<std::wstring, std::string>(std::wstring text) {
        if (text.empty()) return "";
        int bufferSize = WideCharToMultiByte($CSetting.codePageID, 0, text.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (bufferSize <= 0) {
            // 处理转换失败的情况
            return "";
        }
        std::string str(bufferSize - 1, '\0');
        WideCharToMultiByte($CSetting.codePageID, 0, text.c_str(), -1, &str[0], bufferSize, nullptr, nullptr);
        return str;
    }

    // string 转换为 wstring 
    template<>
    static std::wstring Convert<std::string, std::wstring>(std::string text) {
        if (text.empty()) return L"";
        int bufferSize = MultiByteToWideChar($CSetting.codePageID, 0, text.c_str(), -1, nullptr, 0);
        if (bufferSize == 0) {
            // 处理转换失败的情况
            return L"";
        }
        std::wstring wstr(bufferSize - 1, L'\0');
        MultiByteToWideChar($CSetting.codePageID, 0, text.c_str(), -1, &wstr[0], bufferSize);
        return wstr;
    }

    // const WCHAR* 转换为 string
    template<>
    static std::string Convert<const WCHAR*, std::string>(const WCHAR* text) {
        std::wstring wstr(text);
        return std::string(wstr.begin(), wstr.end());
    }

    // WCHAR* 转换为 string
    template<>
    static std::string Convert<WCHAR*, std::string>(WCHAR* text) {
        if (!text) {
            return std::string(); // 如果传入的指针为空，则返回空字符串
        }
        int size = WideCharToMultiByte($CSetting.codePageID, 0, text, -1, nullptr, 0, nullptr, nullptr);
        if (size == 0) {
            // 处理错误，可以记录日志或抛出异常
            return std::string();
        }
        std::string result(size - 1, '\0'); // size - 1 因为 WideCharToMultiByte 包含 null 终止符
        WideCharToMultiByte($CSetting.codePageID, 0, text, -1, &result[0], size, nullptr, nullptr);

        return result;
    }

    // string 转换为 LPCWSTR
    template<>
    static LPCWSTR Convert<std::string, LPCWSTR>(std::string text) = delete;

    // WCHAR 转换为 char
    template<>
    static char Convert<WCHAR, char>(WCHAR wchar) {
        char narrowChar;
        WideCharToMultiByte($CSetting.codePageID, 0, &wchar, 1, &narrowChar, 1, nullptr, nullptr);
        return narrowChar;
    }

    // HWND 转换为 string
    template<>
    static std::string Convert<HWND, std::string>(HWND hwnd) {
        std::stringstream ss;
        ss << "0x" << std::hex << reinterpret_cast<uintptr_t>(hwnd);
        return ss.str();
    }
};