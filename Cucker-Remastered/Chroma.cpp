#include "Chroma.h"

Color Color::Blend(Color a, Color b) {
	if (a == Color{}) return b;
	if (b == Color{}) return a;
	Color result;
	float Alpha = static_cast<float>(b.Alpha) / 255.0f;
	result.Red = static_cast<BYTE>((Alpha * b.Red) + ((1.0f - Alpha) * a.Red));
	result.Green = static_cast<BYTE>((Alpha * b.Green) + ((1.0f - Alpha) * a.Green));
	result.Blue = static_cast<BYTE>((Alpha * b.Blue) + ((1.0f - Alpha) * a.Blue));
	auto sum = std::min(a.Alpha + b.Alpha,255);
	result.Alpha = sum;
	return result;
}

Color Color::FromHex(std::string rgba) {
	if (rgba.size() != 6 && rgba.size() != 8) {
		throw std::invalid_argument("Invalid color string length. Must be 6 (RGB) or 8 (RGBA) characters.");
	}

	// 将十六进制字符串转换为字节
	auto hexToByte = [](const std::string& hex) -> BYTE {
		return static_cast<BYTE>(std::stoi(hex, nullptr, 16));
	};

	Color color;
	color.Red = hexToByte(rgba.substr(0, 2));   // 前两个字符 -> 红
	color.Green = hexToByte(rgba.substr(2, 2)); // 接下来的两个字符 -> 绿
	color.Blue = hexToByte(rgba.substr(4, 2));  // 接下来的两个字符 -> 蓝
	color.Alpha = (rgba.size() == 8) ? hexToByte(rgba.substr(6, 2)) : 255; // 如果有 Alpha，则取，否则默认 255

	return color;
}

void Pixel::GetANSI(char* code,Color bg, Color fg) {
	char commonbuf[4]{};
	strcat(code, "\033[48;2;");
	itoa_2(bg.Red, commonbuf);
	strcat(code, commonbuf);
	strcat(code, ";");

	itoa_2(bg.Green, commonbuf);
	strcat(code, commonbuf);
	strcat(code, ";");

	itoa_2(bg.Blue, commonbuf);
	strcat(code, commonbuf);
	strcat(code, "m");
	strcat(code, "\033[38;2;");

	itoa_2(fg.Red, commonbuf);
	strcat(code, commonbuf);
	strcat(code, ";");

	itoa_2(fg.Green, commonbuf);
	strcat(code, commonbuf);
	strcat(code, ";");

	itoa_2(fg.Blue, commonbuf);
	strcat(code, commonbuf);
	strcat(code, "m");
}

std::string Pixel::GetANSI(Color bg, Color fg) {
	std::string ansi;
	ansi.append("\033[48;2;");
	ansi.append(std::to_string(bg.Red));
	ansi.push_back(';');
	ansi.append(std::to_string(bg.Green));
	ansi.push_back(';');
	ansi.append(std::to_string(bg.Blue));
	ansi.push_back('m');

	ansi.append("\033[38;2;");
	ansi.append(std::to_string(fg.Red));
	ansi.push_back(';');
	ansi.append(std::to_string(fg.Green));
	ansi.push_back(';');
	ansi.append(std::to_string(fg.Blue));
	ansi.push_back('m');
	return ansi;
}