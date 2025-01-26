#pragma once
#include <ostream>

class NullStream : public std::ostream {
public:
    NullStream() : std::ostream(&nullBuffer) {}
private:
    class NullStreamBuffer : public std::streambuf {
    protected:
        virtual int overflow(int c) { return c; }  // �����κβ���
    };

    NullStreamBuffer nullBuffer;
};
