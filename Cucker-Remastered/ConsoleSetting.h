#pragma once
#include <fcntl.h>    // For _O_U16TEXT
#include <windows.h>  // For Windows-specific API

// Const
struct ConsoleSetting {
	DWORD ioStreamMode = _O_U8TEXT;

	UINT codePageID = 936; // GBK

	const char* folderPath = "log";

	DWORD inputEventMAX = 10000;
};
extern ConsoleSetting $CSetting;