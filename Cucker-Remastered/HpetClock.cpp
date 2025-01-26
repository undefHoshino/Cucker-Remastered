#include "HpetClock.h"

void HpetClockModule::Timer::start() {
	startPoint = std::chrono::steady_clock::now();
}
void HpetClockModule::Timer::reset() {
	start();
}
std::chrono::steady_clock::duration HpetClockModule::Timer::Duration() {
	return std::chrono::steady_clock::now() - startPoint;
}

void HpetClockModule::Pause::pause() {
	if (!paused) {
		startPoint = std::chrono::steady_clock::now();
		paused = true;
	}
}
void HpetClockModule::Pause::resume() {
	paused = false;
}
bool HpetClockModule::Pause::isPaused() {
	return paused;
}
std::chrono::steady_clock::duration HpetClockModule::Pause::Duration() {
	std::chrono::steady_clock::duration duration = std::chrono::steady_clock::now() - startPoint;
	return paused ?
		duration :
		std::chrono::steady_clock::duration(0);
}

void HpetClockModule::Rollback::rollback(int64_t rollbackTime) {
	rollbackDuration = std::chrono::milliseconds(rollbackTime);
}
std::chrono::steady_clock::duration HpetClockModule::Rollback::Duration() {
	std::chrono::steady_clock::duration temp = rollbackDuration;
	rollbackDuration = std::chrono::steady_clock::duration(0);
	return temp;
}

void HpetClock::Start() {
	pause.resume();
	timer.start();
}
void HpetClock::Reset() {
	Start();
	speed = 1.0;
}
void HpetClock::Pause() {
	pause.pause();
}
void HpetClock::Resume() {
	timer.getTimePoint() += pause.Duration(); // Move startPoint
	pause.resume();
}
void HpetClock::Rollback(int64_t rollbackTime) {
	rollback.rollback(rollbackTime);
	timer.getTimePoint() -= rollback.Duration();
}
void HpetClock::Reverse() {
	SetSpeed(-speed);
}

void HpetClock::SetSpeed(double newSpeed) {
	if (newSpeed != speed) {
		if (!pause.isPaused()) {
			auto duration = timer.Duration();
			timer.getTimePoint() = std::chrono::steady_clock::now() - std::chrono::duration_cast<std::chrono::steady_clock::duration>(
				duration * speed / newSpeed);
		}
		this->speed = newSpeed;
	}
}
double HpetClock::getSpeed() {
	return this->speed;
}
bool HpetClock::isPaused() {
	return pause.isPaused();
}
bool HpetClock::isReversed() {
	return this->speed < 0;
}
void HpetClock::syncFromMaxDuration() {
	if (getProgress(true) >= 1.0) {
		auto now = std::chrono::steady_clock::now();
		timer.getTimePoint() = now - MaxDuration;
		pause.getTimePoint() = now;
	}
}