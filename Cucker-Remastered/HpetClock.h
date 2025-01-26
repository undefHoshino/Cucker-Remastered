#pragma once
#include <chrono>

class HpetClockModule {
public:
	class Module {
	protected:
		std::chrono::steady_clock::time_point startPoint;
	public:
		virtual std::chrono::steady_clock::duration Duration() = 0;
		virtual std::chrono::steady_clock::time_point& getTimePoint() { return startPoint; };
	};

	class Timer : public Module {
	private:
		double speed = 1.0;
	public:
		void start();
		void reset();
		std::chrono::steady_clock::duration Duration() override;
	};

	class Pause : public Module {
	private:
		bool paused;
	public:
		Pause(bool paused = false) :paused(paused) {};
		void pause();
		void resume();
		bool isPaused();
		std::chrono::steady_clock::duration Duration() override;
	};

	class Rollback : public Module {
	private:
		std::chrono::steady_clock::duration rollbackDuration = std::chrono::steady_clock::duration(0);
	public:
		void rollback(int64_t rollbackTime);
		std::chrono::steady_clock::duration Duration() override;
	};
};

class HpetClock {
private:
	std::chrono::steady_clock::duration MaxDuration;

	HpetClockModule::Timer timer;
	HpetClockModule::Pause pause;
	HpetClockModule::Rollback rollback;

	double speed = 1.0;
	bool reversePaused = false;
public:
	HpetClock(int64_t duration = 1000) : MaxDuration(std::chrono::milliseconds(duration)) {}

	void Start();
	void Reset();
	void Pause();
	void Resume();
	void Rollback(int64_t rollbackTime);
	void Reverse();
	void SetSpeed(double newSpeed);
	double getSpeed();
	bool isPaused();
	bool isReversed();
	void syncFromMaxDuration();

	template<class Duration = std::chrono::milliseconds>
	void setDuration(int64_t duration) {
		auto now = std::chrono::steady_clock::now();
		timer.getTimePoint() = (now - std::chrono::duration(Duration(duration)));
		pause.getTimePoint() = now;
	}
	template<class Duration = std::chrono::milliseconds>
	void setMaxDuration(int64_t duration) {
		MaxDuration = Duration(duration);
	}
	template<class Duration = std::chrono::milliseconds>
	Duration getMaxDuration() {
		return std::chrono::duration_cast<Duration>(MaxDuration);
	}
	template<class Duration = std::chrono::milliseconds>
	Duration getDuration() {
		auto duration = (timer.Duration() - pause.Duration()) * speed;
		return std::chrono::duration_cast<Duration>(duration);
	}
	template<class Duration = std::chrono::milliseconds>
	Duration getElapsedTime(bool overflow = false) {
		auto duration = getDuration<Duration>();
		return overflow ? duration : std::min(getDuration<Duration>(), std::chrono::duration_cast<Duration>(MaxDuration));
	}
	template<class Duration = std::chrono::milliseconds>
	double getProgress(bool overflow = false, bool allowNegative = false) {
		double progress = (double)getElapsedTime<Duration>(overflow).count() / (double)std::chrono::duration_cast<Duration>(MaxDuration).count();
		if (!overflow)
			return std::max(allowNegative ? -1.0 : 0.0, progress);
		return progress;
	}
};