#pragma once
#include <functional>
#include <Windows.h>
#include "Signal.h"
#include "ScreenBuffer.h"
#include "ConsoleFunction.h"
#include "EventDispatcher.h"

class CanvasA;

template<class BufferType>
class IScreenBlender {
public:
	virtual void Blend(BufferType& buf,void* args) = 0;
};

template<class BufferType>
class AbstractScreen {
protected:
	BufferType buffer;
	IScreenBlender<BufferType>* blender;
public:
	AbstractScreen(IScreenBlender<BufferType>* blend):blender(blend) {};
	virtual void resize(int width, int height) {
		buffer.clear(width * height);
		buffer.Width = width;
		buffer.Height = height;
	}
	virtual void cleanBuffer(size_t capacity) {
		buffer.clear(capacity);
	}
	virtual void free() {
		buffer.free();
	}
	virtual void write() = 0;

	int getWidth() {
		return buffer.Width;
	}
	int getHeight() {
		return buffer.Height;
	}
	BufferType& getBuffer() {
		return buffer;
	}
	IScreenBlender<BufferType>& getBlender() {
		return *blender;
	}
protected:
	void setBlender(IScreenBlender<BufferType>* blend) {
		if (this->blender != nullptr)
			_removeBlender();
		this->blender = blend;
	}
	void _removeBlender() {
		delete this->blender;
		this->blender = nullptr;
	}
};

class ScreenBlenderA :public IScreenBlender<ScreenBufferA>{
	//The shit of Fix ANSI Position Measure
	bool Measure0(size_t Index, size_t MultiCharIndex);
	bool checkMeasure(ScreenBufferA& buffers, size_t& index, size_t& mStart, size_t& mEnd, Once& once_Start, Once& once_End, bool& MultiByte);
	void Blend(ScreenBufferA& buf, void* args) override;
};

class ScreenBlenderFixed :public IScreenBlender<ScreenBufferA> {
public:
	int getPosX(ScreenBufferA& buffer, size_t& index,int64_t offset) {
		return (index - offset) % buffer.Width;
	}
	int FixIndexPos(ScreenBufferA& buffer, size_t& index, int64_t offset) {
		int width = GetUnicodeWidth(buffer.buf[index - offset]);
		if (width == -2) {
			std::string last = buffer.buf.substr(index - 3, 4);
			size_t pos = index - 3;
			for (auto c : last) {
				int w = GetUnicodeWidth(c);
				if (w <= 1) continue;
				else {
					index = pos;
					return w - 1;
				}
				pos++;
			}
		} else if(width >= 2){
			return width - 1;
		}
		return -1;
	}
	// Skipped something
	void Blend(ScreenBufferA& buffer, void* args) override {
		Pixel* basePixel = static_cast<Pixel*>(args);
		size_t index = 0;
		int64_t offset = 0;
		Pixel lastPixel;
		Once first;
		//false - Even | true - Odd
		for (auto& pixel : buffer.pixels) {
			if (!first() && lastPixel == pixel) {
				index++;
				continue; //skip
			}
			lastPixel = pixel;

			// Blend
			pixel.bg = Color::Blend(basePixel->bg, pixel.bg);
			pixel.fg = (pixel.fg == Color{}) ? basePixel->fg : Color::Blend(pixel.bg, pixel.fg);

			// Input
			char code[40]{};
			Pixel::GetANSI(code, pixel.bg, pixel.fg);
			int tailSpace = FixIndexPos(buffer, index, offset);
			buffer.buf.insert(index, code);

			if (tailSpace > 0) {
				size_t closeWidthIndex = index + (buffer.Width - getPosX(buffer, index, offset));
				buffer.buf.insert(closeWidthIndex, tailSpace, ' ');
				offset += tailSpace;
				index += tailSpace;
			}

			offset += strlen(code);
			index += strlen(code) + 1; // Add mainChar length
		}
	}
};

class ScreenA :public AbstractScreen<ScreenBufferA> {
public:
	enum Signal {
		Enable,
		Disable,
		Shrink,
		SetBlender,
		SetBasePixel
	};
private:
	SignalManager signManager;
	bool active = true;

	Pixel basePixel = { {0,0,0,255},{255,255,255,255} };
public:
	ScreenA(IScreenBlender<ScreenBufferA>* blend) :AbstractScreen(blend) {};
	void sendSignal(Signal signal, void* args);
	void handleSignal();
	void execute();
	void write() override;
protected:
	void cleanConsoleBuffer();
	void setPos();
	void handles(Signal signal, void* args);
};




//class ScreenBlender {
//public:
//	virtual void Blend(ScreenBuffer& buf, void* args) = 0;
//};
//
//class AbstractScreenParent {
//protected:
//	ScreenBuffer buffer;
//	ScreenBlender* blender;
//public:
//	AbstractScreenParent(ScreenBlender* blend) :blender(blend) {};
//	virtual void resize(int width, int height) {
//		buffer.clear(width * height);
//		buffer.Width = width;
//		buffer.Height = height;
//	}
//	virtual void cleanBuffer(size_t capacity) {
//		buffer.clear(capacity);
//	}
//	virtual void free() {
//		buffer.free();
//	}
//	virtual void write() = 0;
//
//	int getWidth() {
//		return buffer.Width;
//	}
//	int getHeight() {
//		return buffer.Height;
//	}
//	ScreenBuffer& getBuffer() {
//		return buffer;
//	}
//	ScreenBlender& getBlender() {
//		return *blender;
//	}
//protected:
//	void setBlender(ScreenBlender* blend) {
//		if (this->blender != nullptr) {
//			delete this->blender;
//			this->blender = nullptr;
//		}
//		this->blender = blend;
//	}
//};
//
//class Screen :public AbstractScreenParent {
//public:
//	enum struct Event {
//		Enable,
//		Disable,
//		Shrink,
//		SetBlender,
//		SetBasePixel
//	};
//private:
//	EventVector<int> eventVector;
//	bool active = true;
//
//	Pixel basePixel = { {0,0,0,255},{255,255,255,255} };
//public:
//	Screen(ScreenBlender* blend) :AbstractScreenParent(blend) {};
//	void sendEvent(Event event, void* args) {
//		eventVector.push((int)event, args);
//	}
//	void handleSignal() {
//		if (eventVector.empty()) return;
//		for (auto& ptr : signManager.get()) {
//			handles(static_cast<Signal>(ptr.first), ptr.second);
//		}
//		signManager.clear();
//	}
//	void execute() {
//
//	}
//	void write() override {
//
//	}
//protected:
//	void cleanConsoleBuffer() {
//
//	}
//	void setPos() {
//
//	}
//	void handles(Event event, void* args) {
//
//	}
//};