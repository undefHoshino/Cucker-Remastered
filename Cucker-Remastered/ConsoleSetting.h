#pragma once
#include <fcntl.h>    // For _O_U16TEXT
#include <windows.h>  // For Windows-specific API

// Const
struct ConsoleSetting {
	DWORD logStreamMode = _O_U8TEXT;

	UINT codePageID = CP_UTF8; // UTF-8

	const char* folderPath = "log";

	DWORD inputEventMAX = 10000;
};
extern ConsoleSetting $CSetting;