#include "Logger.h"

void Logger::appendAnsi(std::string& msg, Level level) {
    if (AnsiOutput) {
        msg.insert(0, levelAnsi[level]);
        msg.append("\033[0m");
    }
}

std::string Logger::format(Level level) {
    std::string msg = "";
    msg.push_back('[');
    msg.append(currentDateTime());
    msg.append("] [");
    msg.append(levelToString(level));
    msg.append("] ");
    if (!className.empty()) {
        msg.append("[" + className + "] ");
    }
    return msg;
}


// 将日志级别转换为字符串
std::string Logger::levelToString(Level level) {
    switch (level) {
    case Debug: return "DEBUG";
    case Info: return "INFO";
    case Warn: return "WARN";
    case Error: return "ERROR";
    case Fatal: return "FATAL";
    }
    return "UNKNOWN";
}

// 获取当前日期时间
std::string Logger::currentDateTime() {
    std::time_t now = std::time(nullptr);
    char buf[80];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

LogStream Logger::logStream;

bool Logger::AnsiOutput = false;

std::string Logger::levelAnsi[5] = {
        Pixel::GetANSI({ 0,0,0,255 }, { 255,255,255,255 }),     // Debug
        Pixel::GetANSI({ 0,0,0,255 }, { 255,255,255,255 }),     // Info
        Pixel::GetANSI({ 0,0,0,255 }, { 255,255,128,255 }),     // Warn
        Pixel::GetANSI({ 0,0,0,255 }, { 255,80,80,255 }),       // Error
        Pixel::GetANSI({ 0,0,0,255 }, { 255,20,20,255 })          // Fatal
};