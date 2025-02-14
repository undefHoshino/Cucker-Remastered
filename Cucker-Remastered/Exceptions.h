#pragma once

#include <iostream>
#include <exception>
#include <string>

class ReportException : public std::exception {
private:
    std::string message = "null";
public:
    ReportException() = default;

    template<class ...Args>
    ReportException(Args... args) {
        message = toString(args...);
    }

    ReportException& className(const std::string& className) {
        message.insert(message.begin(), '[');
        message.insert(message.begin() + 1, className.begin(), className.end());
        message.insert(message.begin() + 1 + className.length(), ']');
        return *this;
    }

    // 重写 what() 方法，返回异常信息
    const char* what() const noexcept override {
        return message.c_str();
    }
protected:
    template<class ...Args>
    std::string toString(Args... args) {
        std::ostringstream ostream;
        (ostream << ... << args);
        return ostream.str();
    }
};