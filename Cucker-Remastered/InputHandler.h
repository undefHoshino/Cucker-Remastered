#pragma once
#include <Windows.h>
#include <functional>
#include "BitStorage.h"

struct MouseWheelArgs {
	enum WheelDirection {
		None,
		Vertical,
		Horizontal,
	} Direction = None;
	double Delta = 0.0;
};

struct MouseEventArgs {
	COORD position;

	//0   1    2    3    4
	//L   L2   L3   L4   R
	BitStorage<BYTE> buttonState;

	MouseWheelArgs wheel;

	//0     1     2     3
	//MOV   DC    VW    HW
	BitStorage<BYTE> eventflags;

	DWORD controlKeyState = 0;
};

class MouseEventHandler {
public:
    class ButtonPressHandler {
    public:
        struct Result {
            bool pressed = false;
            bool hold = false;
            BitStorage<BYTE> buttonStateRecord;
        };
        void Arrange(MouseEventArgs args) {
            timing = false;
            result.pressed = args.buttonState.get() != 0;
            if (result.pressed) {
                result.hold = true;
                result.buttonStateRecord = args.buttonState;
            }
            else if (!result.pressed && result.hold) {
                result.hold = false;
                timing = true;
            }
        }
        void Cancel() {
            result.hold = false;
        }
        bool Timing() {
            return timing;
        }
        Result& getResult() {
            return result;
        }
    private:
        Result result;
        bool timing = false;
    };
private:
    static short highshort(DWORD dword) {
		return static_cast<short>((dword & 0xffff0000) >> 16);
	}
public:
    static MouseEventArgs Translate(MOUSE_EVENT_RECORD record) {
		MouseEventArgs args;

		BitStorage<DWORD> eventFlags(record.dwEventFlags);

		args.position = record.dwMousePosition;

		if (eventFlags.bit(0)) { // MOUSE_MOVED
			args.eventflags.bit(0, 1); // Moved
		}

		if (eventFlags.bit(1)) { // DOUBLE_CLICK:
			args.eventflags.bit(1, 1); // Double Click
		}
		if (eventFlags.bit(2)) { // MOUSE_WHEELED:
			args.eventflags.bit(2, 1); // Wheeled - Vertical

			args.wheel.Delta = highshort(record.dwButtonState) / 120.0;
			args.wheel.Direction = MouseWheelArgs::Vertical;
		}
		if (eventFlags.bit(3)) { // MOUSE_HWHEELED:
			args.eventflags.bit(3, 1); // HWheeled - Horizontal

			args.wheel.Delta = highshort(record.dwButtonState) / 120.0;
			args.wheel.Direction = MouseWheelArgs::Horizontal;
		}
		
		args.controlKeyState = record.dwControlKeyState;

		BitStorage<DWORD> buttonState(record.dwButtonState);

		//FROM_LEFT_1ST_BUTTON_PRESSED
		if (buttonState.bit(0)) { 
			args.buttonState.bit(0, 1);
		}
		//RIGHTMOST_BUTTON_PRESSED
		if (buttonState.bit(1)) {
			args.buttonState.bit(4, 1);
		}
		//FROM_LEFT_2ND_BUTTON_PRESSED
		if (buttonState.bit(2)) {
			args.buttonState.bit(1, 1);
		}
		//FROM_LEFT_3RD_BUTTON_PRESSED
		if (buttonState.bit(3)) {
			args.buttonState.bit(2, 1);
		}
		//FROM_LEFT_4TH_BUTTON_PRESSED
		if (buttonState.bit(1) && buttonState.bit(3)) {
			args.buttonState.bit(3, 1);
		}
		return args;
	}
};


enum class VirtualKeyCode {
    // Control keys
    BackSpace = 8,      // Backspace key
    Tab = 9,            // Tab key
    Clear = 12,         // Clear key (NumPad 5 with NumLock off)
    Enter = 13,         // Enter key
    Shift = 16,         // Shift key
    Ctrl = 17,          // Ctrl key
    Alt = 18,           // Alt key
    Pause = 19,         // Pause key
    CapsLock = 20,      // Caps Lock key
    Escape = 27,        // Escape key
    Space = 32,         // Spacebar
    PageUp = 33,        // Page Up key
    PageDown = 34,      // Page Down key
    End = 35,           // End key
    Home = 36,          // Home key
    LeftArrow = 37,     // Left Arrow key
    UpArrow = 38,       // Up Arrow key
    RightArrow = 39,    // Right Arrow key
    DownArrow = 40,     // Down Arrow key
    Select = 41,        // Select key
    Print = 42,         // Print key
    Execute = 43,       // Execute key
    PrintScreen = 44,   // Print Screen key
    Insert = 45,        // Insert key
    Delete = 46,        // Delete key
    Help = 47,          // Help key

    // Number keys 0 - 9
    D0 = 48,
    D1 = 49,
    D2 = 50,
    D3 = 51,
    D4 = 52,
    D5 = 53,
    D6 = 54,
    D7 = 55,
    D8 = 56,
    D9 = 57,

    // Letters A-Z
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,

    // NumPad keys
    Numpad0 = 96,
    Numpad1 = 97,
    Numpad2 = 98,
    Numpad3 = 99,
    Numpad4 = 100,
    Numpad5 = 101,
    Numpad6 = 102,
    Numpad7 = 103,
    Numpad8 = 104,
    Numpad9 = 105,

    // NumPad special keys
    Multiply = 106,     // NumPad *
    Add = 107,          // NumPad +
    Separator = 108,    // NumPad Enter
    Subtract = 109,     // NumPad -
    Decimal = 110,      // NumPad .
    Divide = 111,       // NumPad /

    // Function keys F1-F24
    F1 = 112,
    F2 = 113,
    F3 = 114,
    F4 = 115,
    F5 = 116,
    F6 = 117,
    F7 = 118,
    F8 = 119,
    F9 = 120,
    F10 = 121,
    F11 = 122,
    F12 = 123,
    F13 = 124,
    F14 = 125,
    F15 = 126,
    F16 = 127,
    F17 = 128,
    F18 = 129,
    F19 = 130,
    F20 = 131,
    F21 = 132,
    F22 = 133,
    F23 = 134,
    F24 = 135,

    // Lock keys
    NumLock = 144,      // Num Lock key
    ScrollLock = 145,   // Scroll Lock key

    // Modifier keys
    LeftShift = 160,    // Left Shift key
    RightShift = 161,   // Right Shift key
    LeftCtrl = 162,     // Left Ctrl key
    RightCtrl = 163,    // Right Ctrl key
    LeftAlt = 164,      // Left Alt key
    RightAlt = 165,     // Right Alt key

    // Browser keys
    BrowserBack = 166,          // Browser Back key
    BrowserForward = 167,       // Browser Forward key
    BrowserRefresh = 168,       // Browser Refresh key
    BrowserStop = 169,          // Browser Stop key
    BrowserSearch = 170,        // Browser Search key
    BrowserFavorites = 171,     // Browser Favorites key
    BrowserHome = 172,          // Browser Home key

    // Media keys
    VolumeMute = 173,           // Volume Mute key
    VolumeDown = 174,           // Volume Down key
    VolumeUp = 175,             // Volume Up key
    NextTrack = 176,            // Next Track key
    PreviousTrack = 177,        // Previous Track key
    StopMedia = 178,            // Stop Media key
    PlayPauseMedia = 179,       // Play/Pause Media key

    // Application launch keys
    LaunchMail = 180,           // Launch Mail key
    SelectMedia = 181,          // Select Media key
    LaunchApplication1 = 182,   // Launch Application 1 key
    LaunchApplication2 = 183,   // Launch Application 2 key

    // OEM keys (often depend on keyboard layout)
    Oem1 = 186,                 // OEM 1 key        ;:  (semicolon on US keyboards)
    OemPlus = 187,              // OEM Plus key     =+  (equals sign on any keyboard)
    OemComma = 188,             // OEM Comma key    ,<  (comma on any keyboard)
    OemMinus = 189,             // OEM Minus key    -_  (minus sign on any keyboard)
    OemPeriod = 190,            // OEM Period key   .>  (period on any keyboard)
    Oem2 = 191,                 // OEM 2 key        /?  (forward slash on US keyboards)
    Oem3 = 192,                 // OEM 3 key        `~  (grave accent on US keyboards)
    Oem4 = 219,                 // OEM 4 key        [{  (left bracket on US keyboards)
    Oem5 = 220,                 // OEM 5 key        /|  (backslash on US keyboards)
    Oem6 = 221,                 // OEM 6 key        ]}  (right bracket on US keyboards)
    Oem7 = 222,                 // OEM 7 key        '"  (single quote on US keyboards)
    Oem8 = 223,                 // OEM 8 key
    Oem102 = 226,               // OEM 102 key

    // Miscellaneous keys
    Process = 229,              // IME Process key
    Packet = 231,               // Used to pass Unicode characters as if they were keystrokes
    Attn = 246,                 // Attn key
    Crsel = 247,                // Crsel key
    Exsel = 248,                // Exsel key
    EraseEof = 249,             // Erase EOF key
    Play = 250,                 // Play key
    Zoom = 251,                 // Zoom key
    NoName = 252,               // NoName key
    Pa1 = 253,                  // PA1 key
    OemClear = 254              // Clear key
};

struct KeyEventArgs {
    bool keyPressed;
    int repeatCount;

    int scanCode;
    VirtualKeyCode keyCode;

    wchar_t unicodeChar;
    char   asciiChar;

    DWORD controlKeyState;
};

class KeyEventHandler {
public:
    static KeyEventArgs Translate(KEY_EVENT_RECORD record) {
        KeyEventArgs args;
        args.keyPressed = record.bKeyDown;
        args.controlKeyState = record.dwControlKeyState;
        args.repeatCount = record.wRepeatCount;
        args.keyCode = (VirtualKeyCode)record.wVirtualKeyCode;
        args.scanCode = record.wVirtualScanCode;

        args.asciiChar = record.uChar.AsciiChar;
        args.unicodeChar = record.uChar.UnicodeChar;

        return args;
    }
};

struct WinFocusEventArgs {
    bool focus;
};

class WinFocusEventHandler {
public:
    static WinFocusEventArgs Translate(FOCUS_EVENT_RECORD record) {
        return { (bool)record.bSetFocus };
    }
};

struct WinMenuEventArgs {
    UINT commandId;
};

class WinMenuEventHandler {
public:
    static WinMenuEventArgs Translate(MENU_EVENT_RECORD record) {
        return { record.dwCommandId };
    }
};

struct WinBufferEventArgs {
    COORD size;
};

class WinBufferEventHandler {
public:
    static WinBufferEventArgs Translate(WINDOW_BUFFER_SIZE_RECORD record) {
        return { record.dwSize };
    }
};