#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include "Chroma.h"
#include "NulStream.h"

class LogStream {
public:
    // 设置标准输出流
    void setStdoutStream(std::ostream& newStdoutStream) {
        stdoutStream = &newStdoutStream;
    }

    // 设置错误输出流
    void setStderrStream(std::ostream& newStderrStream) {
        stderrStream = &newStderrStream;
    }

    // 获取标准输出流
    std::ostream& Stdout() {
        return *stdoutStream;
    }

    // 获取错误输出流
    std::ostream& Stderr() {
        return *stderrStream;
    }

private:
    std::ostream* stdoutStream;  // 用于标准输出流
    std::ostream* stderrStream;  // 用于错误输出流

public:
    LogStream() : stdoutStream(&std::cout), stderrStream(&std::cerr) {}

    // 禁止拷贝构造和赋值操作
    LogStream(const LogStream&) = delete;
    LogStream& operator=(const LogStream&) = delete;
};

class Logger {
public:
    enum Level {
        Debug = 0,
        Info = 1,
        Warn = 2,
        Error = 3,
        Fatal = 4
    };
protected:
    static std::string levelAnsi[5];
    static LogStream logStream;
    static bool AnsiOutput;

    std::string className = "";
public:
    Logger() {};
    Logger(std::string className) :className(className) {};

    // 日志记录方法，根据日志级别输出
    template<class ...Args>
    void log(Level level, Args... args) {
        std::string msg = format(level) + toString(args...);
        logStream.Stderr() << msg << std::endl;

        appendAnsi(msg,level);
        logStream.Stdout() << msg << std::endl;
    }

    template<class ...Args>
    void debug(Args... args) {
#ifdef _DEBUG
        log(Debug, args...);
#endif
    }

    template<class ...Args>
    void info(Args... args) {
        log(Info, args...);
    }

    template<class ...Args>
    void warn(Args... args) {
        log(Warn, args...);
    }

    template<class ...Args>
    void error(Args... args) {
        log(Error, args...);
    }

    template<class ...Args>
    void fatal(Args... args) {
        log(Fatal, args...);
    }

    void SetClassName(std::string name) {
        this->className = name;
    }

    void SetAnsiOutput(bool f) {
        this->AnsiOutput = f;
    }
protected:
    template<class ...Args>
    std::string toString(Args... args) {
        std::ostringstream ostream;
        (ostream << ... << args);
        return ostream.str();
    }

    void appendAnsi(std::string& msg, Level level);

    // 格式化日志消息
    std::string format(Level level);

    // 将日志级别转换为字符串
    std::string levelToString(Level level);

    // 获取当前日期时间
    std::string currentDateTime();
};