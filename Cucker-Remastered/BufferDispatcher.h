#pragma once
#include "Canvas.h"

class BufferDispatcherA : public AbstractCanvas<ScreenBufferA> {
public:
	BufferDispatcherA() = default;
	BufferDispatcherA(AbstractScreen<ScreenBufferA>* parent) :AbstractCanvas(parent) {};

	void SendBufferData(ScreenBufferA buf, int offsetX, int offsetY, int borderLeft, int borderRight, int borderTop, int borderBottom,
		bool charOverlay = true, Pixel::BlendMode blend = Pixel::Replace, float Alpha = 1.0f);
protected:
	bool stringFix(int x, int y, std::string& str);
};