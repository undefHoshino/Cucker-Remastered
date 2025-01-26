#pragma once

#include <iostream>
#include <exception>
#include <string>

class ReportException : public std::exception {
private:
    std::string message;
public:
    ReportException(const std::string& msg) : message(msg) {}

    // 重写 what() 方法，返回异常信息
    const char* what() const noexcept override {
        return message.c_str();
    }
};