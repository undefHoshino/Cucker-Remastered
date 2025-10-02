#pragma once
#include <fcntl.h>    // For _O_U16TEXT
#include <windows.h>  // For Windows-specific API

// Const
struct ConsoleSetting {
	DWORD logStreamMode = _O_U8TEXT;

	UINT codePageID = 936; // GBK
	const char* folderPath = "log";

	DWORD inputEventMAX = 10000;

	inline void SetVisual(bool flag) {
		visualConsole = flag;
	}

	inline bool IsVisual() {
		return visualConsole;
	}
protected:
	bool visualConsole = false;
};
extern ConsoleSetting $CSetting;