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
    std::ostream* stdoutStream;  // ���ڱ�׼�����
    std::ostream* stderrStream;  // ���ڴ��������

    Logger()
        : stdoutStream(&std::cout), stderrStream(&std::cerr) {}

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
public:
    static Logger& GetInstance();

    // ���ñ�׼�����
    void setStdoutStream(std::ostream& newStdoutStream);

    // ���ô��������
    void setStderrStream(std::ostream& newStderrStream);

    // ��־��¼������������־�������
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

    // ��ʽ����־��Ϣ
    std::string formatMessage(Level level);

    // ����־����ת��Ϊ�ַ���
    std::string levelToString(Level level);

    // ��ȡ��ǰ����ʱ��
    std::string currentDateTime();
};
