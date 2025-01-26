#pragma once
#include <iostream>
#include <io.h>       // For _setmode()
#include "Date.h"
#include "Converter.h"
#include "Interface.h"
#include "InputHandler.h"
#include "ConsoleSetting.h"
#include "ConsoleThread.h"

class ConsoleEngine {
protected:
	LogInstance& logInstance;
	ConsoleThread consoleThread;
	Display display;
public:
	ConsoleEngine() :display(&logInstance.ToLogOverlay()), logInstance(LogInstance::GetInstance()) { };
	virtual void Initialization();
	virtual void RedirectIOStream();
	virtual void Run();
	LogInstance& GetLogger() {
		return logInstance;
	}
protected:
	// Register interface and Navigate
	virtual void Register() = 0;
	virtual void Render(ConsoleThread::IOSignal* signalIO);
	virtual void InputWorker(ConsoleThread::IOSignal* signalIO);

	virtual void initThreads();
	virtual void setLogWritten();
	virtual void setMode();
	virtual bool setVisual();
};