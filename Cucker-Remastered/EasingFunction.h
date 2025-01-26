#pragma once
#include <cmath>
#include <concepts>
#include <corecrt_math_defines.h>
#include <functional>
class EasingFunction {
public:
	class Linear {
	public:
		static double EaseIn(double t) {
			return t;
		}
		static double EaseOut(double t) {
			return t;
		}
		static double EaseInOut(double t) {
			return t;
		}
	};
	class Sine {
	public:
		static double EaseIn(double t) {
			return 1 - std::cos((t * M_PI) / 2);
		}
		static double EaseOut(double t) {
			return std::sin((t * M_PI) / 2);
		}
		static double EaseInOut(double t) {
			return 0.5 * (1 - std::cos(M_PI * t));
		}
	};
	template<double N>
	class Pow {
	public:
		static double EaseIn(double t) {
			return std::pow(t, N);
		}
		static double EaseOut(double t) {
			return 1 - std::pow(1 - t, N);
		}
		static double EaseInOut(double t) {
			if (t < 0.5) {
				return 0.5 * std::pow(2 * t, N); 
			}
			else {
				return 1 - 0.5 * std::pow(2 * (1 - t), N); 
			}
		}
	};
	class Quad  : public Pow<2.0> {};
	class Cubic : public Pow<3.0> {};
	class Quart : public Pow<4.0> {};
	class Quint : public Pow<5.0> {};
	class Expo {
	public:
		static double EaseIn(double t) {
			return t == 0.0 ? 0 : std::pow(2, 10 * (t - 1));
		}
		static double EaseOut(double t) {
			return t == 1.0 ? 1 : 1 - std::pow(2, -10 * t);
		}
		static double EaseInOut(double t) {
			if (t == 0.0) return 0;
			if (t == 1.0) return 1;
			if (t < 0.5) return std::pow(2, 20 * t - 10) / 2;
			return 1 - std::pow(2, -20 * t + 10) / 2;
		}
	};
	class Circ {
	public:
		static double EaseIn(double t) {
			return 1 - std::sqrt(1 - std::pow(t, 2));
		}
		static double EaseOut(double t) {
			return std::sqrt(1 - std::pow(t - 1, 2));
		}
		static double EaseInOut(double t) {
			return t < 0.5 ? (1 - std::sqrt(1 - std::pow(2 * t, 2))) / 2 : (std::sqrt(1 - std::pow(-2 * t + 2, 2)) + 1) / 2;
		}
	};
	class Back {
	public:
		static double EaseIn(double t) {
			return std::pow(t, 2) * ((2.70158 * t) - 1.70158);
		}
		static double EaseOut(double t) {
			return 1 + std::pow(t - 1, 2) * ((2.70158 * (t - 1)) + 1.70158);
		}
		static double EaseInOut(double t) {
			double s = 1.5;
			t = 2 * t;
			if (t < 1) {
				return 0.5 * (t * t * ((s + 1) * t - s));
			}
			else {
				t -= 2;
				return 0.5 * (t * t * ((s + 1) * t + s) + 2);
			}
		}
		
	};
	class Elastic {
	public:
		static double EaseIn(double t) {
			if (t == 0.0) return 0;
			if (t == 1.0) return 1;

			double p = 0.3;  // 振荡周期
			double s = p / 4;  // 初始相位

			return -(std::pow(2, 10 * (t - 1)) * std::sin((t - 1 - s) * (2 * M_PI) / p));
		}
		static double EaseOut(double t) {
			if (t == 0.0) return 0;
			if (t == 1.0) return 1;

			double p = 0.3;  // 振荡周期
			double s = p / 4;

			return std::pow(2, -10 * t) * std::sin((t - s) * (2 * M_PI) / p) + 1;
		}
		static double EaseInOut(double t) {
			if (t == 0.0) return 0;
			if (t == 1.0) return 1;

			double p = 0.3;  // 振荡周期
			double s = p / 4;

			if (t < 0.5) {
				return -0.5 * std::pow(2, 10 * (2 * t - 1)) * std::sin((2 * t - 1 - s) * (2 * M_PI) / p);
			}
			else {
				return std::pow(2, -10 * (2 * t - 1)) * std::sin((2 * t - 1 - s) * (2 * M_PI) / p) * 0.5 + 1;
			}
		}
	};
	class Bounce {
	public:
		static double EaseIn(double t) {
			return 1.0 - EaseOut(1.0 - t);
		}
		static double EaseOut(double t) {
			if (t < 1 / 2.75) {
				return 7.5625 * t * t;
			}
			else if (t < 2 / 2.75) {
				t -= 1.5 / 2.75;
				return 7.5625 * t * t + 0.75;
			}
			else if (t < 2.5 / 2.75) {
				t -= 2.25 / 2.75;
				return 7.5625 * t * t + 0.9375;
			}
			t -= 2.625 / 2.75;
			return 7.5625 * t * t + 0.984375;

		}
		static double EaseInOut(double t) {
			if (t < 0.5) {
				return EaseIn(t * 2) * 0.5;
			}
			else {
				return EaseOut(t * 2 - 1) * 0.5 + 0.5;
			}
		}
	};
	class Flash {
	public:
		static double EaseIn(double t, int flashes = 3) {
			return std::pow(t, 2) * std::sin(t * flashes * M_PI);
		}

		static double EaseOut(double t, int flashes = 3) {
			return (1 - std::pow(1 - t, 2)) * std::sin(t * flashes * M_PI);
		}

		static double EaseInOut(double t, int flashes = 3) {
			if (t < 0.5) {
				return 0.5 * EaseIn(t * 2, flashes);
			}
			else {
				return 0.5 * EaseOut(t * 2 - 1, flashes) + 0.5;
			}
		}
	};
	template<std::function In, std::function Out, std::function InOut>
	class Custom {
	public:
		static double EaseIn(double t) {
			return In(t);
		}

		static double EaseOut(double t) {
			return Out(t);
		}

		static double EaseInOut(double t) {
			return InOut(t);
		}
	};
};


/// <param name="type">
/// : EaseIn 0 
/// | EaseOut 1
/// | EaseInOut 2
/// </param>
template<class Ease>
static std::function<double(double)> GetEasingFunction(int type) {
	switch (type) {
	case 0:
		return [](double t) -> double {
			return Ease::EaseIn(t);
		};
	case 1:
		return [](double t) -> double {
			return Ease::EaseOut(t);
		};
	case 2:
		return [](double t) -> double {
			return Ease::EaseInOut(t);
		};
	default:
		throw std::runtime_error("Unknown EaseType: " + std::to_string(type));
	};
};