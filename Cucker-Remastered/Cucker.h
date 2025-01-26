#pragma once
#include "Engine.h"
#include "TestScreen.h"
#include "TimerScreen.h"
#include "MainScreen.h"
#include "AdofaiSetupScreen.h"
#include "AdofaiTweaksChartScreen.h"
class Cucker : public ConsoleEngine {
protected:
	void Register() override {
		// Register interface and Navigate
		display.Insert(0, MakeMainScreen());
		display.Insert(1, MakeTestScreen());
		display.Insert(2, MakeTimerScreen());
		display.Insert(3, MakeAdofaiSetupScreen());			
		display.Insert(4, MakeAdofaiTweaksChartScreen());
		display.Navigate(3);
	}
};