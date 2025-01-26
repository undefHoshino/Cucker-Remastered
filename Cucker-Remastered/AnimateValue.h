#pragma once
#include "HpetClock.h"
#include "EasingFunction.h"


template <typename T>
class ValueUpdater {
public:
	ValueUpdater() = default;
	ValueUpdater(T initialValue) : currentValue(initialValue), lastValue(initialValue) {}

	void initial(T initialValue) {
		currentValue = initialValue;
		lastValue = initialValue;
	}

	bool update(T newValue) {
		if (newValue != lastValue) {
			currentValue = newValue;
			lastValue = newValue;
			return true; // 值发生了变化
		}
		return false; // 值没有变化
	}

	T getValue() const {
		return currentValue;
	}

private:
	T currentValue; // 当前值
	T lastValue;    // 上一次值
};

template<class T>
class AnimateValue {
public:
	using Easing = std::function<double(double)>;
private:
	ValueUpdater<T> valueUpdater;
	HpetClock clock;
	Easing easing = [](double t) { return t; };

	T start = 0;
	T end = 0;
public:
	AnimateValue(T value) {
		setup(value);
		setupClock();
	}
	void setEasing(Easing easing) {
		this->easing = easing;
	}
	void setDuration(uint64_t duration) {
		clock.setMaxDuration(duration);
	}
protected:
	void setupClock() {
		clock.setMaxDuration(500);
		clock.Start();
	}
	void setup(T value) {
		start = value;
		end = value;
		valueUpdater.initial(value);
	}
	T interpolate() {
		double progress = easing(getProgress());
		return ((end - start) * progress) + start;
	}
public:
	double getProgress() {
		return clock.getProgress();
	}
	double getEasedProgress() {
		return easing(getProgress());
	}
	T getValue() {
		return interpolate();
	}
	T getStart() {
		return start;
	}
	T getEnd() {
		return end;
	}
	T operator()() {
		return interpolate();
	}
	void operator=(T num) {
		if (valueUpdater.update(num)) {
			start = interpolate();
			end = num;
			clock.Reset();
		}
	}
};