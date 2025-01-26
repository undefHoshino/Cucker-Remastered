#include "Defines.h"

void Pause() {
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    FlushConsoleInputBuffer(hInput);
    char _temp = _getch();
}
