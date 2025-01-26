#pragma once
#include <algorithm>
#include "Screen.h"
#include "Point.h"
#include "Converter.h"
template<class BufferType>
class AbstractCanvas {
protected:
	AbstractScreen<BufferType>* parent = nullptr;
	BufferType* buffer = nullptr;
public:
	AbstractCanvas() = default;
	AbstractCanvas(AbstractScreen<BufferType>* parent) :parent(parent),buffer(&parent->getBuffer()) {};
	virtual void attach(AbstractScreen<BufferType>* parent) {
		if (parent == nullptr) return;
		this->parent = parent;
		this->buffer = &parent->getBuffer();
	}
protected:
	size_t getPos(int x, int y) {
		return static_cast<size_t>(y) * buffer->Width + x;
	}
	size_t getPos(int x, int y, int width) {
		return static_cast<size_t>(y) * width + x;
	}
	bool checkPos(int x,int y) {
		return x < buffer->Width && y < buffer->Height;
	}
	void fixPos(int& x, int& y) {
		if (x < 0) x = 0;
		if (y < 0) y = 0;
		x = std::min(x, buffer->Width);
		y = std::min(y, buffer->Height);
	}
};

class CanvasA :public AbstractCanvas<ScreenBufferA> {
public:
	CanvasA() = default;
	CanvasA(AbstractScreen<ScreenBufferA>* parent) :AbstractCanvas(parent){};

	void SetPixel(int x, int y, Pixel pixel, char c = '\0');
	void DrawString(int x, int y, std::string str, Pixel pixel);
	void DrawLineV(int x, int y1, int y2, Pixel pixel, char c = '\0', bool ignore = false);
	void DrawLineH(int x1, int x2, int y, Pixel pixel, char c = '\0', bool ignore = false);
	void DrawLine(int x1, int y1, int x2, int y2, Pixel pixel, char c = '\0');
	void FillRect(int x1, int y1, int x2, int y2, Pixel pixel, char c = '\0');
	void FillPolygon(std::vector<PointI> vertices, Pixel pixel, char c = '\0');
protected:
	void pointIFix(PointI& a, PointI& b);
	bool stringFix(int &x, int &y, std::string &str);
};