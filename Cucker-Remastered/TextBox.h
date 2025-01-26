#pragma once
#include "WidgetBase.h"
#include "TextInputControl.h"

class TextBox : public ActionableWidget {
public:
	struct Properties : public RectProperties {
		Pixel cursorColor = { {200,200,200,50},{} };
		Pixel rectColor = { {200,200,200,50},{} };
		Properties() = default;
		Properties(int x, int y, UINT width, UINT height, Pixel cursorColor = { {200,200,200,50},{} }, Pixel rectColor = { {200,200,200,50},{} }) :
			RectProperties(x, y, width, height), cursorColor(cursorColor), rectColor(rectColor) {};
	};
	class Style : public WidgetStyle {
	private:
		bool drawLineNum = false;
		bool shouldFillEntireRect = true;
	public:
		virtual void DrawPrimitive(ScreenA& screen, CanvasA& canvas, Widget* widget) override;
		virtual void MouseEvent(MouseEventArgs* args, Widget* self) override;
		virtual void KeyEvent(KeyEventArgs* args, Widget* self) override;
		void DrawBox(ScreenA& screen, CanvasA& canvas, TextBox* textBox);
		void DrawSelection(ScreenA& screen, CanvasA& canvas, TextBox* textBox, int textBoxX, int textBoxY);
		void setLineNumberVisibility(bool visible);
		void setShouldFillEntireRect(bool flag);
	};
protected:
	TextInputControl inputControl;
public:
	void Init() override;
	void SetMaxLine(size_t maxLine);
	void setLineNumberVisibility(bool visible);
	void setShouldFillEntireRect(bool flag);
	std::string GetString();
	std::wstring GetWstring();
};