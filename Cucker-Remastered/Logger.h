#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include "Chroma.h"
#include "NulStream.h"

class LogStream {
public:
    // ���ñ�׼�����
    void setStdoutStream(std::ostream& newStdoutStream) {
        stdoutStream = &newStdoutStream;
    }

    // ���ô��������
    void setStderrStream(std::ostream& newStderrStream) {
        stderrStream = &newStderrStream;
    }

    // ��ȡ��׼�����
    std::ostream& Stdout() {
        return *stdoutStream;
    }

    // ��ȡ���������
    std::ostream& Stderr() {
        return *stderrStream;
    }

private:
    std::ostream* stdoutStream;  // ���ڱ�׼�����
    std::ostream* stderrStream;  // ���ڴ��������

public:
    LogStream() : stdoutStream(&std::cout), stderrStream(&std::cerr) {}

    // ��ֹ��������͸�ֵ����
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

    // ��־��¼������������־�������
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

    // ��ʽ����־��Ϣ
    std::string format(Level level);

    // ����־����ת��Ϊ�ַ���
    std::string levelToString(Level level);

    // ��ȡ��ǰ����ʱ��
    std::string currentDateTime();
};