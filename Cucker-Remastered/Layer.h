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
    virtual void Focus(FocusEventArgs args) {}
    virtual void Menu(MenuEventArgs args) {}
    virtual void BufferSize(BufferEventArgs args) {}
};