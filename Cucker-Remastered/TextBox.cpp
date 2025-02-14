#include "TextBox.h"

void TextBox::Style::DrawPrimitive(ScreenA& screen, CanvasA& canvas, Widget* widget) {
	auto* textbox = widget->as<TextBox>();
	DrawBox(screen, canvas, textbox);
}

void TextBox::Style::MouseEvent(MouseEventArgs* args, Widget* widget) {
	auto* textbox = widget->as<TextBox>();
	auto* prop = widget->getProperties<Properties>();
	auto& inputer = textbox->inputControl;
	int judgeX = prop->X;
	int judgeY = prop->Y;
	int judgeWidth = prop->Width;
	int judgeHeight = prop->Height;

	if (shouldFillEntireRect) {
		judgeHeight = prop->Height;
	}
	else {
		judgeHeight = inputer.getBuffer().size();
	}

	if (drawLineNum) {
		judgeX += 2;
		judgeWidth += 2;
	}

	bool inArea = InputCondition::Mouse::InArea(judgeX, judgeY, judgeWidth, judgeHeight, *args);

	if (inArea) {
		if (args->buttonState[0]) {
			inputer.enable();
			textbox->callEvent(Events::onTextboxActive, args, true);
		}
	}

};

void TextBox::Style::KeyEvent(KeyEventArgs* args, Widget* widget) {
	auto* textbox = widget->as<TextBox>();
	auto& inputer = textbox->inputControl;
	if (args->keyCode == VirtualKeyCode::Escape) {
		inputer.disable();
	}
	inputer.KeyEvent(*args);
};

void TextBox::Style::DrawBox(ScreenA& screen, CanvasA& canvas, TextBox* textBox) {
	auto& inputer = textBox->inputControl;
	auto& prop = *textBox->getProperties<Properties>();
	auto& buffer = inputer.getBuffer();
	auto& anchor = inputer.getAnchor();
	int lineNumLen = std::to_string(buffer.size()).size() + 1;
	int textBoxX = prop.X;
	int textBoxY = prop.Y;

	if (drawLineNum) {
		textBoxX += lineNumLen;
	}

	int _height = prop.Y;
	if (shouldFillEntireRect) {
		_height += prop.Height;
	}
	else {
		_height += buffer.size();
	}
	Pixel textColor = { {},{255,255,255,255} };
	Pixel rectcolor = prop.rectColor;

	canvas.FillRect(prop.X, prop.Y, prop.X + prop.Width, _height, prop.rectColor);
	for (size_t i = 0; i < buffer.size(); i++) {
		std::wstring wstr(buffer.getLine(i).data().begin(), buffer.getLine(i).data().end());
		if (drawLineNum) {
			canvas.DrawString(prop.X, textBoxY + i, std::to_string(i), textColor);
			canvas.SetPixel(textBoxX - 1, textBoxY + i, textColor, '|');
		}
		canvas.DrawString(textBoxX, textBoxY + i, Converter::Convert<std::wstring, std::string>(wstr), textColor);
	}

	if (!inputer.isEnabled()) return;
	if (anchor.isActive()) {
		DrawSelection(screen, canvas, textBox, textBoxX, textBoxY);
	}
	else {
		canvas.SetPixel(textBoxX + inputer.getCursorPosition().column, textBoxY + inputer.getCursorPosition().row, prop.cursorColor);
	}
}

void TextBox::Style::DrawSelection(ScreenA& screen, CanvasA& canvas, TextBox* textBox, int textBoxX, int textBoxY) {
	auto& inputer = textBox->inputControl;
	auto& prop = *textBox->getProperties<Properties>();
	auto& buffer = inputer.getBuffer();
	auto& anchor = inputer.getAnchor();
	size_t count = inputer.getSelectedLineCount();
	size_t startX = anchor.startColumn();
	size_t endX = anchor.endColumn();
	size_t startY = anchor.startRow();
	size_t endY = anchor.endRow();
	if (startY == endY) {
		canvas.DrawLineH(textBoxX + startX, textBoxX + endX, startY + textBoxY, prop.cursorColor);
		return;
	}
	size_t widthTop = textBoxX + buffer.getLine(startY).size();
	if (buffer.getLine(startY).size() == 0) {
		widthTop++;
	}
	canvas.DrawLineH(textBoxX + startX, widthTop, startY + textBoxY, prop.cursorColor);

	for (size_t y = startY + 1; y < endY; ++y) {
		size_t widthMiddle = textBoxX + buffer.getLine(y).size();
		if (buffer.getLine(y).size() == 0) {
			widthMiddle++;
		}
		canvas.DrawLineH(textBoxX, widthMiddle, y + textBoxY, prop.cursorColor);
	}

	size_t widthBottom = textBoxX + endX;
	if (buffer.getLine(endY).size() == 0) {
		widthBottom++;
	}
	canvas.DrawLineH(textBoxX, widthBottom, endY + textBoxY, prop.cursorColor);
}

void TextBox::Style::setLineNumberVisibility(bool visible) {
	this->drawLineNum = visible;
}

void TextBox::Style::setShouldFillEntireRect(bool flag) {
	this->shouldFillEntireRect = flag;
}

void TextBox::Init() {
	Widget::Init();
	SetStyle(new Style());
	addEvent(_MouseEvent_, [this](void* _args, ActionableWidget* self) { style->MouseEvent(static_cast<MouseEventArgs*>(_args), self); });
	addEvent(_KeyEvent_, [this](void* _args, ActionableWidget* self) { style->KeyEvent(static_cast<KeyEventArgs*>(_args), self); });

	SetDrawer([this](ScreenA* screen, CanvasA* canvas, KeyFrameGroupLoader loader, void* args) {
		style->DrawPrimitive(*screen, *canvas, this);
		});
}

void TextBox::SetMaxLine(size_t maxLine) {
	inputControl.setMaxLine(maxLine);
}

void TextBox::setLineNumberVisibility(bool visible) {
	getStyle<Style>()->setLineNumberVisibility(visible);
}

void TextBox::setShouldFillEntireRect(bool flag) {
	getStyle<Style>()->setShouldFillEntireRect(flag);
}

std::string TextBox::GetString() {
	return inputControl.getString();
}

std::wstring TextBox::GetWstring() {
	return inputControl.getWstring();
}

TextInputControl& TextBox::getInputControl() {
	return inputControl;
}