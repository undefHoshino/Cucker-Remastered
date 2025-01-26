#pragma once
#include <chrono>
using namespace std::chrono;

class Timer
{
private:
    time_point<high_resolution_clock> m_begin;
public:
    Timer() : m_begin(high_resolution_clock::now()) {}
    void begin() {
        m_begin = high_resolution_clock::now();
    }
    //Ĭ���������
    int64_t elapsed() const
    {
        return duration_cast<milliseconds>(high_resolution_clock::now() - m_begin).count();
    }
    //΢��
    int64_t elapsed_micro() const
    {
        return duration_cast<microseconds>(high_resolution_clock::now() - m_begin).count();
    }
    //����
    int64_t elapsed_nano() const
    {
        return duration_cast<nanoseconds>(high_resolution_clock::now() - m_begin).count();
    }
    //��
    int64_t elapsed_seconds() const
    {
        return duration_cast<seconds>(high_resolution_clock::now() - m_begin).count();
    }
    //��
    int64_t elapsed_minutes() const
    {
        return duration_cast<minutes>(high_resolution_clock::now() - m_begin).count();
    }
    //ʱ
    int64_t elapsed_hours() const
    {
        return duration_cast<hours>(high_resolution_clock::now() - m_begin).count();
    }
    //��
    int64_t elapsed_days() const
    {
        return duration_cast<days>(high_resolution_clock::now() - m_begin).count();
    }
    //��
    int64_t elapsed_weeks() const
    {
        return duration_cast<weeks>(high_resolution_clock::now() - m_begin).count();
    }
    //��
    int64_t elapsed_months() const
    {
        return duration_cast<months>(high_resolution_clock::now() - m_begin).count();
    }
    //��
    int64_t elapsed_years() const
    {
        return duration_cast<years>(high_resolution_clock::now() - m_begin).count();
    }
};