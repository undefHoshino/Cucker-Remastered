#pragma once
#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <string>

void SetConcurPos(COORD position);

COORD GetConcurPos();

CONSOLE_SCREEN_BUFFER_INFO GetConsoleScreenBuffer();

int GetConsoleWindowWidth(CONSOLE_SCREEN_BUFFER_INFO& csbi);

int GetConsoleWindowHeight(CONSOLE_SCREEN_BUFFER_INFO& csbi);

void FillConsoleBuffer(COORD pos, DWORD cellCount);

void FillConsoleBuffer(COORD pos, DWORD cellCount, WORD attributes);

bool CheckConsoleInstance();

bool RestoreConsoleInstance();

void IgnoreCtrlCExit(bool enable);