#pragma once
#include "Interface.h"
#include "BufferDispatcher.h"

class Overlay : public Interface {
protected:
	struct Border {
		int left;
		int right;
		int top;
		int bottom;
	};
	struct {
		bool charOverlay = true;
		Pixel::BlendMode blend = Pixel::Replace;
		float Alpha = false;
	} attribute;

	BufferDispatcherA dispatcher;
	Border border;
	int overlayWidth = 0, overlayHeight = 0, overlayX = 0, overlayY = 0;
public:
	Overlay() : dispatcher(nullptr) {};
	void OverlayCreation(ScreenA* main, LogOverlay* logger, Display* display, int width, int height, int x = 0, int y = 0);
	virtual void OverlayRender();
	virtual void onCreateScreen() override;
	virtual void onWriteConsole() override;
	void freeOverlay();
	void setPosition(int x, int y);
	void setAttribute(bool wCharOverlay = true, Pixel::BlendMode wBlend = Pixel::Replace, float wAlpha = 1.0f);
	void resize(int width, int height);
	void setBorder(int left, int right, int top, int bottom) {
		border.left = left;
		border.right = right;
		border.top = top;
		border.bottom = bottom;
	}
};