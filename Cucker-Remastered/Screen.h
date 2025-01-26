#pragma once
#include <functional>
#include <Windows.h>
#include "Signal.h"
#include "ScreenBuffer.h"
#include "ConsoleFunction.h"

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