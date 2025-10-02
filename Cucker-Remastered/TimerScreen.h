#pragma once
#include <iomanip>
#include <sstream>
#include "Interface.h"
#include "HpetClock.h"
class TimerScreen : public Interface {
private:
	Pixel textColor = { {},{255,255,255,255} };
	HpetClock clock;
	FpsOverlay fpsOverlay;

	bool togglePauseResume = false;
	bool toggleReversed = false;
	double speed = 1.0;
public:
	void Creation(ScreenA* screen, CanvasA* canvas, Logger* logger, Displayer* display) override {
		Interface::Creation(screen, canvas, logger, display);
		clock.setMaxDuration<std::chrono::years>(100);
	}
	std::string getStandardTime(int64_t milliseconds) {
		bool negative = false;
		if (milliseconds < 0) {
			negative = true;
			milliseconds = std::abs(milliseconds);
		}

		int64_t total_seconds = milliseconds / 1000;
		int64_t seconds = total_seconds % 60;
		int64_t total_minutes = total_seconds / 60;
		int64_t minutes = total_minutes % 60;
		int64_t hours = total_minutes / 60;

		milliseconds %= 1000;

		std::ostringstream timeStream;
		if (negative) timeStream << "- ";
		timeStream << hours << "h "
			<< minutes << "m "
			<< seconds << "s " 
			<< milliseconds  << "ms";

		return timeStream.str();
	}
	void Render() override {
		canvas->DrawString(2, 1, "Clock", textColor);
		int textLineX2Min = screen->getWidth() > 4 + 5 ? screen->getWidth() - 2 : 4 + 5;
		canvas->DrawLineH(2, textLineX2Min, 1, { {230,230,230,100},{} });
		int64_t duration = clock.getDuration<std::chrono::milliseconds>().count();

		canvas->DrawString(2, 3, "Duration: ", textColor);
		canvas->DrawString(4, 5, "Standard:  " + getStandardTime(duration), textColor);
		canvas->DrawString(4, 6, "HpetClock: " + std::to_string(duration), textColor);

		canvas->DrawString(2, 16, "Information", textColor);
		canvas->DrawLineH(2, textLineX2Min, 16, { {230,230,230,100},{} });

		canvas->DrawString(2, 18, "Clock Speed: " + std::to_string(clock.getSpeed()), textColor);


		fpsOverlay.Render(*screen, *canvas);
	}
	void Key(KeyEventArgs args) override {
		if (args.keyPressed) {
			if (args.keyCode == VirtualKeyCode::Space) {
				if (!togglePauseResume) {
					clock.Pause();
					logger->info("Paused");
				}
				else {
					clock.Resume();
					logger->info("Resume");
				}
				togglePauseResume = !togglePauseResume;
			}
			if (args.keyCode == VirtualKeyCode::Q) {
				clock.syncFromMaxDuration();
				logger->info("syncFromMaxDuration");
			}
			if (args.keyCode == VirtualKeyCode::S) {
				clock.Start();
				logger->info("Start");
				togglePauseResume = false;
			}
			if (args.keyCode == VirtualKeyCode::E) {
				int64_t duration = clock.getMaxDuration<std::chrono::milliseconds>().count();
				clock.setDuration(duration);
				logger->info("SetDuration(Max): ", duration);
			}
			if (args.keyCode == VirtualKeyCode::R) {
				clock.Reverse();
				if (!toggleReversed) {
					logger->info("Reversed");
				}
				else {
					logger->info("Normal");
				}
				toggleReversed = !toggleReversed;
			}
			if (args.keyCode == VirtualKeyCode::A) {
				speed = clock.getSpeed();
				speed -= 0.1;
				clock.SetSpeed(speed);
				logger->info("Speed: ", speed);
			}
			if (args.keyCode == VirtualKeyCode::D) {
				speed = clock.getSpeed();
				speed += 0.1;
				clock.SetSpeed(speed);
				logger->info("Speed: ", speed);
			}
			if (args.keyCode == VirtualKeyCode::LeftArrow) {
				clock.Rollback(-1000);
				logger->info("Rollback -1000ms");
			}
			if (args.keyCode == VirtualKeyCode::RightArrow) {
				clock.Rollback(1000);
				logger->info("Rollback 1000ms");
			}
			if (args.keyCode == VirtualKeyCode::W) {
				clock.Reset();
				logger->info("Reset");
			}
		}
	}
};

TimerScreen* MakeTimerScreen();