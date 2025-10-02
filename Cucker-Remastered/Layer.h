#pragma once
#include "Screen.h"
#include "Canvas.h"
#include "InputHandler.h"

class Layer {
public:
	virtual void Render(ScreenA& screen, CanvasA& canvas) {};
    virtual void Update() {};
    virtual void Mouse(MouseEventArgs args) {}
    virtual void Key(KeyEventArgs args) {}
    virtual void WinFocus(WinFocusEventArgs args) {}
    virtual void WinMenu(WinMenuEventArgs args) {}
    virtual void WinBufferSize(WinBufferEventArgs args) {}
};