#include "Engine.h"

//void ConsoleEngine::RedirectIOStream() {
//	freopen("CONOUT$", "w", stdout);  // 将stdout重定向到新控制台
//	freopen("CONIN$", "r", stdin);    // 重定向stdin
//}
//
//void ConsoleEngine::setLogWritten() {
//	if (CreateDirectoryA($CSetting.folderPath, NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
//		// 设置 stderr 的字符集为 UTF-8
//		_setmode(_fileno(stderr), $CSetting.ioStreamMode);
//
//        
//        std::string logname = "log/log" + Date::getFormattedTime("_%Y-%m-%d_%H-%M-%S") + ".txt";
//		// 将 stderr 重定向到文件
//		freopen(logname.c_str(), "w", stderr);
//	}
//	else {
//		logInstance.ToLogger().error("Unable to create folder: ", GetLastError());
//	}
//}
//
//void ConsoleEngine::Initialization() {
//    Register();
//}
//
//void ConsoleEngine::Run() {
//    while (true) {
//        try {
//            display.BackgroundLogic();
//            consoleThread.Supervisor();
//        }
//        catch (std::exception e) {
//            logger.fatal("Exception: ",e.what());
//        }
//        Sleep(1);
//    }
//}
//
//void ConsoleEngine::setMode() {
//	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
//	unsigned long mode = 0;
//	GetConsoleMode(hIn, &mode);
//	mode |= (ENABLE_MOUSE_INPUT | ENABLE_AUTO_POSITION);
//	mode &= ~(ENABLE_QUICK_EDIT_MODE | ENABLE_INSERT_MODE);
//	SetConsoleMode(hIn, mode);
//}
//
//bool ConsoleEngine::setVisual() {
//	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
//	DWORD dwMode = 0;
//
//	// 尝试获取当前控制台模式
//	if (!GetConsoleMode(hOut, &dwMode))
//	{
//		logInstance.ToLogger().error("GetConsoleMode failed with error ", GetLastError());
//		return false;
//	}
//
//	// 尝试启动ENABLE_VIRTUAL_TERMINAL_PROCESSING
//	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
//	if (!SetConsoleMode(hOut, dwMode))
//	{
//		logInstance.ToLogger().error("SetConsoleMode failed with error ", GetLastError());
//		return false;
//	}
//
//	// 如果上面的代码没有报错，那么可以认为控制台支持ENABLE_VIRTUAL_TERMINAL_PROCESSING特性，即我们的"新"定义
//	logInstance.ToLogger().debug("This console appears to support ENABLE_VIRTUAL_TERMINAL_PROCESSING.");
//
//	return true;
//}
//
//void ConsoleEngine::initThreads() {
//    consoleThread.Create([this](ConsoleThread::IOSignal* signalIO) {
//        Render(signalIO);
//        }, [this](ConsoleThread::IOSignal* signalIO) {
//            InputWorker(signalIO);
//        });
//}
//
//void ConsoleEngine::Render(ConsoleThread::IOSignal* signalIO) {
//    while (signalIO->in != ConsoleThread::Exit) {
//        try {
//            display.Render();
//        }
//        catch (const std::exception& ex) {
//            logInstance.ToLogOverlay().fatal("Render throws an exception: ", ex.what());
//            //MessageBox(NULL, Converter::Convert<string, wstring>(string("Render throws an exception: \n") + ex.what()).c_str(), L"DESTRUCTION!", MB_ICONERROR | MB_OK);
//        }
//        catch (...) {
//            logInstance.ToLogOverlay().fatal("Unknown exception caught");
//            //MessageBox(NULL, L"Unknown Exception in Render.", L"DESTRUCTION!", MB_ICONERROR | MB_OK);
//        }
//    }
//}
//
//void ConsoleEngine::InputWorker(ConsoleThread::IOSignal* signalIO) {
//    DWORD EventMAX = ConsoleSetting().inputEventMAX;
//    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
//	INPUT_RECORD InputRecord;
//    DWORD eventsRead;
//    DWORD NumberOfEvents = 0;
//    while (ReadConsoleInputW(hInput, &InputRecord, 1, &eventsRead)) {
//        try {
//            if (signalIO->in == ConsoleThread::Exit) return;
//
//            GetNumberOfConsoleInputEvents(hInput, &NumberOfEvents);
//            if (NumberOfEvents >= EventMAX) {
//                FlushConsoleInputBuffer(hInput);
//                logInstance.ToLogOverlay().error("Events Overflow.");
//            }
//            switch (InputRecord.EventType) {
//            case MOUSE_EVENT:
//                display.SendInputArgs<MouseEventArgs>(MouseEventHandler::Translate(InputRecord.Event.MouseEvent));
//                break;
//            case KEY_EVENT:
//                display.SendInputArgs<KeyEventArgs>(KeyEventHandler::Translate(InputRecord.Event.KeyEvent));
//                break;
//            case FOCUS_EVENT:
//                display.SendInputArgs<WinFocusEventArgs>(WinFocusEventHandler::Translate(InputRecord.Event.FocusEvent));
//                //if (!InputRecord.Event.FocusEvent.bSetFocus) {
//                //    view.display().logger.LogInfo("[!] Shrinking memory... ");
//                //    view.FreeMemory();
//                //}
//                break;
//            case MENU_EVENT:
//                display.SendInputArgs<WinMenuEventArgs>(WinMenuEventHandler::Translate(InputRecord.Event.MenuEvent));
//                break;
//            case WINDOW_BUFFER_SIZE_EVENT:
//                display.SendInputArgs<WinBufferEventArgs>(WinBufferEventHandler::Translate(InputRecord.Event.WindowBufferSizeEvent));
//                break;
//            }
//        }
//        catch (const std::exception& ex) {
//            logInstance.ToLogger().error(ex.what());
//        }
//        catch (...) {
//            logInstance.ToLogger().error("Error during key.");
//        }
//    }
//    logInstance.ToLogger().fatal("InputWorker unexpectedly exited");
//}
