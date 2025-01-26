#pragma once
#include <concepts>
#include <Windows.h>
#include <conio.h>

class Once {
private:
    bool hasExecuted = false;
public:
    //Once triggered
    bool operator()() {
        if (!hasExecuted) hasExecuted = true;
        else return false;

        return true;
    }
    //Reset status
    void operator~() {
        hasExecuted = false;
    }
};
/*
class ExecuteLock {
public:
    void Run(std::function<void()> func) {
        if (lock) return;
        func();
        lock = true;
    }
    void Unlock() {
        lock = false;
    }
    void Lock() {
        lock = true;
    }
private:
    bool lock = false;
};
*/

class Range {
public:
    // inRange: Check if value is within the range [(start, end)]
    template<std::integral T, bool StartInterval = true, bool EndInterval = true>
    static bool inRange(T start, T end, T value) {
        return (StartInterval ? (value >= start) : (value > start)) &&
            (EndInterval ? (value <= end) : (value < end));
    }

    // limitValue: Limit value to be within the range [start, end]
    template<std::integral T>
    static T limitValue(T start, T end, T value) {
        if (value < start) return start;
        if (value > end) return end;
        return value;
    }
};

template <std::integral Int, typename CharT>
inline void itoa_2(Int num, CharT* out) {
    int j = 0;
    if (num == 0) {
        out[j++] = '0';
        out[j++] = 0;
        return;
    }
    Int num2 = num;
    while (num2 > 0) {
        num2 /= 10;
        j++;
    }
    out[j] = 0;
    j--;
    while (num) {
        out[j--] = "0123456789"[num % 10];
        num /= 10;
    }
}

void Pause();