#pragma once
#include "Engine.h"
#include "Console.h"
#include "ConsoleThread.h"
#include "Displayer.h"
#include "TestScreen.h"
#include "TimerScreen.h"
#include "MainScreen.h"
#include "AdofaiSetupScreen.h"
#include "AdofaiTweaksChartScreen.h"

class Cucker : public ConsoleEngine {
protected:
	Console* console = nullptr;
	ConsoleThread* consoleThread = nullptr;
	Displayer* displayer = nullptr;
public:
	void Initialization() override {
		logger.SetClassName("Cucker");

		console		  = Use<Console>();
		consoleThread = Use<ConsoleThread>();
		displayer	  = Use<Displayer>();
		
		ConsoleEngine::Initialization();
	}
	void Run() override {
		ConsoleEngine::Run();

		while (true) {
			try {
				displayer->BackgroundLogic();
				consoleThread->Supervisor();
			}
			catch (const ReportException& e) {
				logger.fatal("Exception: ", e.what());
				break;
			}
			catch (std::exception& e) {
				logger.fatal("Exception: ", e.what());
				break;
			}
			Sleep(1);
		}
	}
protected:
	void Register() override {
		// Register interface and Navigate
		displayer->Insert("MainScreen", MakeMainScreen());
		displayer->Insert("TestScreen", MakeTestScreen());
		displayer->Insert("TimerScreen", MakeTimerScreen());
		displayer->Insert("AdofaiSetupScreen", MakeAdofaiSetupScreen());
		displayer->Insert("AdofaiTweaksChartScreen", MakeAdofaiTweaksChartScreen());
		displayer->Navigate("TestScreen");
	}
};