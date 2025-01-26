#include "MainScreen.h"

MainScreen* MakeMainScreen() {
	return new MainScreen();
}

std::string MainScreen::GetTimeFormatted() {
	std::string formattedText = "";
	std::time_t t = std::time(nullptr);
	std::tm* tm = std::localtime(&t);
	char buffer[100];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S %A", tm);

	std::string star = (tm->tm_hour > 6 && tm->tm_hour < 18) ? "¡î " : "¡ï ";
	formattedText.append(star);
	formattedText.append(buffer);
	return formattedText;
}