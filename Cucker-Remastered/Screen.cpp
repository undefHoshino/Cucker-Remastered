#include "Screen.h"

void ScreenA::sendSignal(Signal signal, void* args) {
	signManager.push(signal, args);
}
void ScreenA::handleSignal() {
	if (signManager.empty()) return;
	for (auto& ptr : signManager.get()) {
		handles(static_cast<Signal>(ptr.first), ptr.second);
	}
	signManager.clear();
}
void ScreenA::execute() {
	if (!active) return;
	handleSignal();
	setPos();
	blender->Blend(buffer, (void*)&basePixel);
}
void ScreenA::write() {
	WriteConsoleA(
		GetStdHandle(STD_OUTPUT_HANDLE),
		this->buffer.buf.c_str(),
		this->buffer.buf.size(),
		NULL,
		NULL
	);
}

void ScreenA::cleanConsoleBuffer() {
	CONSOLE_SCREEN_BUFFER_INFO csbi = GetConsoleScreenBuffer();
	SHORT consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	DWORD cellCount = (csbi.dwSize.X * csbi.dwSize.Y) - (consoleHeight * csbi.dwSize.X);
	FillConsoleBuffer(COORD{ 0,(SHORT)(consoleHeight - 1) }, cellCount, csbi.wAttributes);
}
void ScreenA::setPos() {
	SetConcurPos({ 0,0 });
}
void ScreenA::handles(Signal signal, void* args) {
	switch (signal) {
	case Signal::Enable:
		active = true;
		break;
	case Signal::Disable:
		active = false;
		break;
	case Signal::Shrink:
		buffer.shrink();
		cleanConsoleBuffer();
		break;
	case Signal::SetBlender:
		setBlender(static_cast<IScreenBlender<ScreenBufferA>*>(args));
		break;
	case Signal::SetBasePixel:
		basePixel = *static_cast<Pixel*>(args);
		break;
	}
}

bool ScreenBlenderA::Measure0(size_t Index, size_t MultiCharIndex) {
	return (Index - MultiCharIndex) % 2 == 0; // Even
}
bool ScreenBlenderA::checkMeasure(ScreenBufferA& buffers, size_t& index, size_t& mStart, size_t& mEnd, Once& once_Start, Once& once_End, bool& MultiByte) {
	if (buffers.buf[index] < 0) {
		if (once_Start()) {
			MultiByte = true;
			mStart = index;
			mEnd = index;
			~once_End;
		}
	}
	else {
		if (once_End()) {
			MultiByte = false;
			mEnd = index;
			~once_Start;
		}
	}
	//Even - true | Odd - false
	if (!Measure0(mEnd, mStart)) {
		mStart = 0;
		mEnd = 0;
		buffers.buf.insert(index, "?");
		index += 2;
		return false;
	}

	return true;
}
void ScreenBlenderA::Blend(ScreenBufferA& buf, void* args) {
	Pixel* basePixel = static_cast<Pixel*>(args);
	size_t index = 0;
	Pixel Last;
	Once Skip;

	size_t mStart = 0;
	size_t mEnd = 0;
	Once once_Start;
	Once once_End;
	bool MultiByte = false;

	//false - Even | true - Odd
	for (auto& pointer : buf.pixels) {
		if (!checkMeasure(buf, index, mStart, mEnd, once_Start, once_End, MultiByte)) continue;

		if (!Skip() && Last == pointer) {
			index++;
			continue; //skip
		}
		Last = pointer;

		// Blend
		pointer.bg = Color::Blend(basePixel->bg, pointer.bg);
		pointer.fg = (pointer.fg == Color{}) ? basePixel->fg : Color::Blend(pointer.bg, pointer.fg);

		// Input
		char code[40]{};
		Pixel::GetANSI(code, pointer.bg, pointer.fg);
		buf.buf.insert((MultiByte && !Measure0(index, mStart)) ? index + 1 : index, code);
		size_t strLength = strlen(code);
		index += strLength + 1; // Add mainChar length
		if (MultiByte) {
			mStart += strLength;
			mEnd += strLength;
		}
	}
}