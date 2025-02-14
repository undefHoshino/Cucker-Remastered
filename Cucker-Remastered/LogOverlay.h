#pragma once
#include <deque>
#include <sstream>
#include <mutex>
#include "Logger.h"
#include "Timer.h"
#include "Layer.h"

class LogOverlay : protected Logger, protected Layer {
protected:
    struct LogEntry {
        Logger::Level level;
        std::string msg;
        int64_t endtime;
    };

    static Pixel colors[5];
    std::deque<LogEntry>logEntries;
    std::mutex dataMutex;

    const int MaxTick = 1600;
    Timer tickTimer;

    LogOverlay() {
        tickTimer.begin();
    }
public:
    LogOverlay(const LogOverlay&) = delete;
    LogOverlay& operator=(const LogOverlay&) = delete;

    static LogOverlay& GetInstance() {
        static LogOverlay instance;  // 静态局部变量，在第一次访问时创建
        return instance;
    }

    template<class ...Args>
    void log(Level level, Args... args) {
        std::string rawMsg = toString(args...);
        logStream.Stderr() << format(level) + rawMsg << std::endl;

        std::lock_guard<std::mutex> lock(dataMutex);
        logEntries.push_front({ level,toString(args...),tickTimer.elapsed() + MaxTick });
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

    void Render(ScreenA& screen, CanvasA& canvas) override;
    void Update() override;
    size_t size() { return logEntries.size(); }
};