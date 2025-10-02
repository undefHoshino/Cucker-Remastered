#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <deque>
#include <mutex>
#include "Chroma.h"
#include "NulStream.h"
#include "Timer.h"
#include "Layer.h"
#include "Unicode.h"

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
    class ILogFormatter {
    public:
        virtual std::string formatPrefix(Level level, std::string className) {
            std::string msg = "[" + date() + "] [" + levelToStr(level) + "] ";
            if (!className.empty()) {
                msg.append("[" + className + "] ");
            }
            return msg;
        }

        virtual std::string date() {
            std::time_t now = std::time(nullptr);
            char buf[80];
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
            return std::string(buf);
        }

        virtual std::string levelToStr(Level level) {
            switch (level) {
            case Debug: return "DEBUG";
            case Info: return "INFO";
            case Warn: return "WARN";
            case Error: return "ERROR";
            case Fatal: return "FATAL";
            }
            return "UNKNOWN";
        }
        virtual ~ILogFormatter() = default;
    };

    class ILogParent {
    protected:
        Logger* parent = nullptr;
        std::shared_ptr<ILogFormatter> formatter = nullptr;
    public:
        ILogParent(const std::shared_ptr<ILogFormatter>& fmt) {
            this->redirect(fmt);
        }
        void redirect(const std::shared_ptr<ILogFormatter>& fmt) {
            this->formatter = fmt;  
        }
        void setParent(Logger* parent) {
            this->parent = parent;
        }
    };

    // static
    class ILogSink: public ILogParent {
    protected:
        static constexpr const char* logANSI[5] = {
            "\033[48;2;0;0;0m\033[38;2;255;255;255m",     // Debug
            "\033[48;2;0;0;0m\033[38;2;255;255;255m",     // Info
            "\033[48;2;0;0;0m\033[38;2;255;255;128m",     // Warn
            "\033[48;2;0;0;0m\033[38;2;255;80;80m",       // Error
            "\033[48;2;0;0;0m\033[38;2;255;20;20m"        // Fatal
        };
        static LogStream logStream;
        static bool withANSI;
    public:
        ILogSink(const std::shared_ptr<ILogFormatter>& fmt): ILogParent(fmt) {}
        virtual void insertANSI(std::string& msg, Level level, std::string end = "\033[0m") {
            if (withANSI) {
                msg.insert(0, logANSI[level]);
                msg.append(end);
            }
        }

        template<class ...Args>
        std::string toString(Args... args) {
            std::ostringstream ostream;
            (ostream << ... << convertToString(std::forward<Args>(args)));
            return ostream.str();
        }

        template<typename T>
        auto convertToString(const T& value) {
            if constexpr (std::is_same_v<T, std::wstring> ||
                std::is_same_v<T, wchar_t*> ||
                std::is_same_v<T, const wchar_t*>)
            {
                return Utf16ToUtf8(std::wstring(value)); // 统一转UTF-8
            }
            else if constexpr (std::is_same_v<T, wchar_t>) {
                return Utf16ToUtf8(std::wstring{ value });
            }
            else {
                return value; // 其他类型原样返回
            }
        }
    public:
        
        template<class ...Args>
        void log(Level level, Args... args) {
            // parent->className!!!
            std::string combined = formatter->formatPrefix(level, parent->className) + toString(args...);
            logStream.Stderr() << combined << std::endl;
            insertANSI(combined, level);
            logStream.Stdout() << combined << std::endl;
        }

        void Ansi(bool flag) {
            this->withANSI = flag;
        }

        virtual ~ILogSink() = default;
    };

    class LogSink : public ILogSink {
    public:
        LogSink(const std::shared_ptr<ILogFormatter>& fmt) : ILogSink(fmt) {}
    };

    // static
    class ILogOverlay : public ILogSink {
    public:
        ILogOverlay(const std::shared_ptr<ILogFormatter>& fmt) : ILogSink(fmt){}
        virtual void Render(ScreenA& screen, CanvasA& canvas) {};
        virtual void Update() {};

        template<class ...Args>
        void log(Level level, Args... args) {
            std::string rawMsg = toString(args...);

            // parent->className!!!
            logStream.Stderr() << formatter->formatPrefix(level, parent->className) + rawMsg << std::endl;
            processLog(level, rawMsg);
        }
        virtual void processLog(Level level, std::string msg) {};
        virtual ~ILogOverlay() = default;
    };

    class LogOverlay: public ILogOverlay {
    public:
        struct LogEntry {
            Logger::Level level;
            std::string message;
            int64_t endtime;
        };
    protected:
        static Pixel logColors[5];

        static std::deque<LogEntry>logEntries;
        static std::mutex dataMute;
        static std::once_flag initFlag;
        static Timer tickTimer;
        static int MaxTick;
    public:
        LogOverlay(const std::shared_ptr<ILogFormatter>& fmt, int maxTick = 1600) : ILogOverlay(fmt) {
            std::call_once(initFlag, []() {
                tickTimer.begin();
            });
            MaxTick = maxTick;
        }
        virtual void processLog(Level level, std::string msg) override {
            std::lock_guard<std::mutex> lock(dataMute);
            logEntries.push_front({ level,msg,tickTimer.elapsed() + MaxTick });
        }

        virtual void Render(ScreenA& screen, CanvasA& canvas) override {
            if (logEntries.size() == 0)
                return;
            int y = screen.getHeight() - 3;
            int64_t time = tickTimer.elapsed();

            for (auto& logs : logEntries) {
                if (time > logs.endtime)
                    continue;
                float Alpha = std::min((static_cast<double>(logs.endtime - time) / static_cast<double>(MaxTick)) * 2, 1.0);
                canvas.DrawString(0, y--, logs.message, logColors[logs.level] * Alpha);
                if (y < 10)
                    break;
            }

            Update();
        }
        virtual void Update() override {
            std::lock_guard<std::mutex> lock(dataMute);

            if (logEntries.size() == 0) return;
            int64_t time = tickTimer.elapsed();
            while (!logEntries.empty()) {
                if (time > logEntries.back().endtime)
                    logEntries.pop_back();
                else
                    break;
            }
        }
        
        static void setMaxTick(int maxTick) {
            MaxTick = maxTick;
        }
    };
    std::string className = "";

    std::shared_ptr<ILogFormatter> logFormatter = nullptr;
    std::shared_ptr<ILogSink> logSink = nullptr;
    std::shared_ptr<ILogOverlay> logOverlay = nullptr;

public:
    Logger() {
        initialize();
    };
    Logger(std::string className) {
        initialize();
        this->className = className;
    };

    void initialize() {
        if (!logFormatter) {
            this->logFormatter = std::make_shared<ILogFormatter>();
            this->logSink = std::make_shared<LogSink>(logFormatter);
            this->logOverlay = std::make_shared<LogOverlay>(logFormatter);
        }
        this->logSink->setParent(this);
        this->logOverlay->setParent(this);
    }

    // 日志记录方法，根据日志级别输出
    template<class ...Args>
    void log(Level level, Args... args) {
        if ($CSetting.IsVisual()) {
            logOverlay->log(level, args...);
        }
        else {
            logSink->log(level, args...);
        }

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

    void SetANSI(bool flag) {
        logSink->Ansi(flag);
        logOverlay->Ansi(flag);
    }

    void LogOverlayRender(ScreenA& screen, CanvasA& canvas) {
        logOverlay->Render(screen, canvas);
    }

    void LogOverlayUpdate() {
        logOverlay->Update();
    }

    void setLogFormatterPtr(std::shared_ptr<ILogFormatter> formatterPtr) {
        this->logSink->redirect(formatterPtr);
        this->logOverlay->redirect(formatterPtr);
    }

    void setLogSinkPtr(std::shared_ptr<ILogFormatter> formatterPtr, std::shared_ptr<ILogSink> sinkPtr) {
        this->logSink = sinkPtr;
        this->logSink->redirect(formatterPtr);
    }

    void setLogOverlayPtr(std::shared_ptr<ILogFormatter> formatterPtr, std::shared_ptr<ILogOverlay> overlayPtr) {
        this->logOverlay = overlayPtr;
        this->logOverlay->redirect(formatterPtr);
    }


};
