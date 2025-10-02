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
	freopen("CONOUT$", "w", stdout);  // ��stdout�ض����¿���̨
	freopen("CONIN$", "r", stdin);    // �ض���stdin
	redirectLogStream();
}

void Console::redirectLogStream() {
	if (CreateDirectoryA($CSetting.folderPath, NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
		// ���� stderr ���ַ���Ϊ UTF-8
		_setmode(_fileno(stderr), $CSetting.logStreamMode);


		std::string logname = "log/log" + Date::getFormattedTime("_%Y-%m-%d_%H-%M-%S") + ".txt";
		// �� stderr �ض����ļ�
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

	// ���Ի�ȡ��ǰ����̨ģʽ
	if (!GetConsoleMode(hOut, &dwMode))
	{
		logger.error("GetConsoleMode failed with error ", GetLastError());
		return false;
	}

	// ��������ENABLE_VIRTUAL_TERMINAL_PROCESSING
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hOut, dwMode))
	{
		logger.error("SetConsoleMode failed with error ", GetLastError());
		return false;
	}

	// �������Ĵ���û�б�����ô������Ϊ����̨֧��ENABLE_VIRTUAL_TERMINAL_PROCESSING���ԣ������ǵ�"��"����
	logger.debug("This console appears to support ENABLE_VIRTUAL_TERMINAL_PROCESSING.");

	return true;
}
