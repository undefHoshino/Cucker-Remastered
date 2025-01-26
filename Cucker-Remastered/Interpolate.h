#pragma once
#include <functional>
#include "HpetClock.h"

template <typename T>
class Interpolator {
public:
	using InterpFunc = std::function<T(T, T, float)>;
	using Pair		 = std::pair<T, InterpFunc*>;
private:
	static InterpFunc interpolate;
public:
	void SetInterpolate(T interpolate) {
		this->interpolate = interpolate;
	}
	Pair Make(T value){
		return std::make_pair(value, &interpolate);
	}
};
