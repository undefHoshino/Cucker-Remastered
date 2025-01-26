#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include "Chroma.h"
#include "NulStream.h"

class Logger {
public:
	enum Level {
		Debug = 0,
		Info  = 1,
		Warn  = 2,
		Error = 3,
		Fatal = 4
	};
protected:
    std::string levelAnsi[5] = {
        Pixel::GetANSI({ 0,0,0,255 }, { 255,255,255,255 }),     // Debug
        Pixel::GetANSI({ 0,0,0,255 }, { 255,255,255,255 }),     // Info
        Pixel::GetANSI({ 0,0,0,255 }, { 255,255,128,255 }),     // Warn
        Pixel::GetANSI({ 0,0,0,255 }, { 255,80,80,255 }),       // Error
        Pixel::GetANSI({ 0,0,0,255 }, { 255,20,20,255 })          // Fatal
    };
    bool AnsiOutput = false;
    std::ostream* stdoutStream;  // 用于标准输出流
    std::ostream* stderrStream;  // 用于错误输出流

    Logger()
        : stdoutStream(&std::cout), stderrStream(&std::cerr) {}

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
public:
    static Logger& GetInstance();

    // 设置标准输出流
    void setStdoutStream(std::ostream& newStdoutStream);

    // 设置错误输出流
    void setStderrStream(std::ostream& newStderrStream);

    // 日志记录方法，根据日志级别输出
    template<class ...Args>
    void log(Level level, Args... args) {
        std::string msg = formatMessage(level) + toString(args...);
        (*stderrStream) << msg << std::endl;

        appendAnsi(msg,level);
        (*stdoutStream) << msg << std::endl;
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

    void ansiOutput(bool f) {
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
    std::string formatMessage(Level level);

    // 将日志级别转换为字符串
    std::string levelToString(Level level);

    // 获取当前日期时间
    std::string currentDateTime();
};
