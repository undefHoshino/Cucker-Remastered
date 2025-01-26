#include "ConsoleFunction.h"

#undef max
#undef min

void SetConcurPos(COORD position) {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
}

COORD GetConcurPos() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = { 0,0 };
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        coordScreen.X = csbi.dwCursorPosition.X;
        coordScreen.Y = csbi.dwCursorPosition.Y;
        return coordScreen;
    }
    else {
        return { -32768,-32768 };
    }
}

CONSOLE_SCREEN_BUFFER_INFO GetConsoleScreenBuffer() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi;
}
int GetConsoleWindowWidth(CONSOLE_SCREEN_BUFFER_INFO& csbi) {
    return csbi.dwSize.X;
}

int GetConsoleWindowHeight(CONSOLE_SCREEN_BUFFER_INFO& csbi) {
    return std::max(0,csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
}

void FillConsoleBuffer(COORD pos, DWORD cellCount) {
    HANDLE hStdOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD _count;

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE) return;
    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;

    if (!FillConsoleOutputCharacter(hStdOut, (TCHAR)' ', cellCount, pos, &_count)) return;
    if (!FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount, pos, &_count)) return;

    SetConsoleCursorPosition(hStdOut, pos);
}
void FillConsoleBuffer(COORD pos, DWORD cellCount,WORD attributes) {
    HANDLE hStdOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD _count;

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE) return;

    if (!FillConsoleOutputCharacter(hStdOut, (TCHAR)' ', cellCount, pos, &_count)) return;
    if (!FillConsoleOutputAttribute(hStdOut, attributes, cellCount, pos, &_count)) return;

    SetConsoleCursorPosition(hStdOut, pos);
}

bool CheckConsoleInstance() {
    return GetConsoleWindow() != NULL;
}

void IgnoreCtrlCExit(bool enable) {
    // 如果禁用 Ctrl+C 退出，移除之前的处理器
    if (!enable) {
        if (SetConsoleCtrlHandler([](DWORD dwCtrlType) -> BOOL {
            exit(0xc000013a); // SB
            return FALSE;
            }, TRUE) == FALSE) {
            throw std::runtime_error("Failed to uninstall Control-C handler!");
        }
    }
    // 如果启用 Ctrl+C 处理，安装新的处理程序
    else {
        if (SetConsoleCtrlHandler(
            [](DWORD dwCtrlType) -> BOOL {
                if (dwCtrlType == CTRL_C_EVENT) {
                    return TRUE; // 忽略 Ctrl+C
                }
                return FALSE;
            }, TRUE) == FALSE) {
            throw std::runtime_error("Failed to install Control-C handler!");
        }
    }
}