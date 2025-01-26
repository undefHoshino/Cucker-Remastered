#include "Logger.h"

Logger& Logger::GetInstance() {
    static Logger instance;  // 静态局部变量，确保只创建一次
    return instance;
}

// 设置标准输出流
void Logger::setStdoutStream(std::ostream& newStdoutStream) {
    stdoutStream = &newStdoutStream;
}

void Logger::setStderrStream(std::ostream& newStderrStream) {
    stderrStream = &newStderrStream;
}

void Logger::appendAnsi(std::string& msg, Level level) {
    if (AnsiOutput) {
        msg.insert(0, levelAnsi[level]);
        msg.append("\033[0m");
    }
}

std::string Logger::formatMessage(Level level) {
    std::string msg = "";
    msg.push_back('[');
    msg.append(currentDateTime());
    msg.append("] [");
    msg.append(levelToString(level));
    msg.append("] ");
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