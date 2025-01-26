#pragma once
#include <deque>
#include <sstream>
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

    const int MaxTick = 1600;
    Timer tickTimer;
    LogOverlay() {
        tickTimer.begin();
    }

    LogOverlay(const LogOverlay&) = delete;
    LogOverlay& operator=(const LogOverlay&) = delete;
public:
    static LogOverlay& GetInstance();

    template<class ...Args>
    void log(Level level, Args... args) {
        std::string rawMsg = toString(args...);
        (*stderrStream) << formatMessage(level) + rawMsg << std::endl;
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


class LogInstance : private LogOverlay {
protected:
    Logger& logger = Logger::GetInstance();
    LogOverlay& logOverlay = LogOverlay::GetInstance();
public:
    static LogInstance& GetInstance();

    // 提供对 Logger 和 LogOverlay 实例的访问
    Logger& ToLogger();
    LogOverlay& ToLogOverlay();
protected:
    LogInstance() {}
    LogInstance(const LogInstance&) = delete;
    LogInstance& operator=(const LogInstance&) = delete;
};