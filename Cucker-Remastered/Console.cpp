#include "Console.h"

void Console::Init(ConsoleEngine& source) {
	ConsoleEngine::Component::Init(source);
	logger.SetClassName("Console");
	SetConsoleOutputCP($CSetting.codePageID);
	redirectLogStream();
	setMode();
	if (!setConsoleVirtual()) {
		logger.error("Failed in setVisual().");
	}
	else {
		logger.SetANSI(true);
	}
}

void Console::redirectIOStream() {
	freopen("CONOUT$", "w", stdout);  // 将stdout重定向到新控制台
	freopen("CONIN$", "r", stdin);    // 重定向stdin
	redirectLogStream();
}

void Console::redirectLogStream() {
	if (CreateDirectoryA($CSetting.folderPath, NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
		// 设置 stderr 的字符集为 UTF-8
		_setmode(_fileno(stderr), $CSetting.logStreamMode);


		std::string logname = "log/log" + Date::getFormattedTime("_%Y-%m-%d_%H-%M-%S") + ".txt";
		// 将 stderr 重定向到文件
		freopen(logname.c_str(), "w", stderr);
	}
	else {
		logger.error("Unable to create folder: ", GetLastError());
	}
}

void Console::setMode() {
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	unsigned long mode = 0;
	GetConsoleMode(hIn, &mode);
	mode |= (ENABLE_MOUSE_INPUT | ENABLE_AUTO_POSITION);
	mode &= ~(ENABLE_QUICK_EDIT_MODE | ENABLE_INSERT_MODE);
	SetConsoleMode(hIn, mode);
}

bool Console::setConsoleVirtual() {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;

	// 尝试获取当前控制台模式
	if (!GetConsoleMode(hOut, &dwMode))
	{
		logger.error("GetConsoleMode failed with error ", GetLastError());
		return false;
	}

	// 尝试启动ENABLE_VIRTUAL_TERMINAL_PROCESSING
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hOut, dwMode))
	{
		logger.error("SetConsoleMode failed with error ", GetLastError());
		return false;
	}

	// 如果上面的代码没有报错，那么可以认为控制台支持ENABLE_VIRTUAL_TERMINAL_PROCESSING特性，即我们的"新"定义
	logger.debug("This console appears to support ENABLE_VIRTUAL_TERMINAL_PROCESSING.");

	return true;
}
