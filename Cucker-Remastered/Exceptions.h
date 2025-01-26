#pragma once

#include <iostream>
#include <exception>
#include <string>

class ReportException : public std::exception {
private:
    std::string message;
public:
    ReportException(const std::string& msg) : message(msg) {}

    // ��д what() �����������쳣��Ϣ
    const char* what() const noexcept override {
        return message.c_str();
    }
};