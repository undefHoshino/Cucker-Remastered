#pragma once
#include <ctime>       // For std::time_t, std::tm, std::localtime
#include <string>      // For std::string

class Date {
public:
	static std::string getFormattedTime(std::string format = "%Y-%m-%d %H:%M:%S") {
		std::time_t now = std::time(nullptr);
		char buf[80];
		std::strftime(buf, sizeof(buf), format.c_str(), std::localtime(&now));
		return std::string(buf);
	}
	static std::string getWeekday(int* wday = nullptr) {
		std::tm tm = getTimeStruct();
		const char* days[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
		if (wday) *wday = tm.tm_wday;
		return days[tm.tm_wday];
	}
	static std::time_t getUnixTimestamp() {
		return std::time(nullptr);  // 返回当前时间的时间戳
	}
	static std::tm getTimeStruct() {
		std::time_t now = std::time(nullptr);
		std::tm tm = *std::localtime(&now);
		return tm;
	}
	static bool isLeapYear(int year) {
		if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
			return true;
		return false;
	}
};