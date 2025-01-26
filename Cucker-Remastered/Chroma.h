#pragma once
#include <string>
#include <stdexcept>
#include "Defines.h"
#undef min

struct Color {
	using BYTE = unsigned char;
	BYTE Red;
	BYTE Green;
	BYTE Blue;
	BYTE Alpha;
	static Color Blend(Color a, Color b);
	static Color FromHex(std::string rgba);
	bool operator==(const Color& b) const {
		return std::memcmp(this, &b, sizeof(Color)) == 0;
	}
	template<class T>
	Color operator*(T x) {
		return Color{ Red,Green,Blue,(BYTE)(Alpha * x) };
	}
	template <class T>
	Color operator^(T x) {
		return Color{ Alpha, (BYTE)(Red * x), (BYTE)(Green * x), (BYTE)(Blue * x) };
	}
	double Difference(Color b) {
		return (std::abs((Red - b.Red) / 255.0) + std::abs((Blue - b.Blue) / 255.0) + std::abs((Green - b.Green) / 255.0)) / 3.0;
	}
	double Brightness() {
		return Red * 0.1 + Green * 0.2 + Blue * 0.7;
	}
};

struct Pixel {
	Color bg;
	Color fg;
	enum BlendMode {
		Replace,
		Mix,
		Discard
	} mode = BlendMode::Mix;
	bool operator==(const Pixel& p) const {
		return this->bg == p.bg && this->fg == p.fg;
	}
	template<class T>
	Pixel operator*(T num) {
		return Pixel{ bg * num, fg * num, this->mode };
	}
	static void GetANSI(char* code, Color bg, Color fg);
	static std::string GetANSI(Color bg, Color fg);
};