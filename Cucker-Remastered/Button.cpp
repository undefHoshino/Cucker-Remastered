#include "Button.h"

void Button::Init() {
	Widget::Init();
	SetStyle(new Style());

	SetDrawer([this](ScreenA* screen, CanvasA* canvas, KeyFrameGroupLoader loader, void* args) {
		style->DrawPrimitive(*screen, *canvas, this);
		});

	addEvent(Events::_MouseEvent_, [this](void* _args, ActionableWidget* self) {
		style->MouseEvent(static_cast<MouseEventArgs*>(_args), self);
		});
}

void Button::Style::DrawPrimitive(ScreenA& screen, CanvasA& canvas, Widget* widget) {
	auto& prop = *widget->getProperties<Properties>();
	Pixel rectColor = prop.rectBoxColor;
	switch (getIndex()) {
	case 1:
		rectColor.bg.Alpha += 60;
		break;
	case 2:
		rectColor.bg.Alpha -= 20;
		break;
	}
	canvas.FillRect(prop.X, prop.Y, prop.X + prop.Width, prop.Y + prop.Height, rectColor);
	PosCalculator calc({ prop.X,prop.Y,(int)prop.Width,(int)prop.Height });

	PointI centerPoint = calc.alignCenter(prop.Caption.length());
	canvas.DrawString(centerPoint.x, centerPoint.y, prop.Caption, { {},{255,255,255,255} });
}

void Button::Style::MouseEvent(MouseEventArgs* args, Widget* widget) {
	auto& prop = *widget->getProperties<Properties>();
	auto& style = *widget->getStyle<Style>();

	bool inArea = InputCondition::Mouse::InArea(prop.X, prop.Y, prop.Width, prop.Height, *args);

	if (inArea) {
		if (args->buttonState[0]) {
			style.setIndex(2);
		}
		else {
			style.setIndex(1);
		}
		return;
	}
	style.setIndex(0);
}

void Button::Style::setIndex(int buttonIndex) {
	this->ButtonIndex = buttonIndex;
}

int Button::Style::getIndex() {
	return ButtonIndex;
}

void NoobButton::Init() {
	Button::Init();
	SetStyle(new Style());
	refreshEvent(Button::_MouseEvent_);
	addEvent(Button::_MouseEvent_, [this](void* _args, ActionableWidget* self) {
		style->MouseEvent(static_cast<MouseEventArgs*>(_args), self);
		});
	addEvent(Button::_KeyEvent_, [this](void* _args, ActionableWidget* self) {
		style->KeyEvent(static_cast<KeyEventArgs*>(_args), self);
		});
}

void NoobButton::Style::MouseEvent(MouseEventArgs* args, Widget* widget) {
	auto& actionable = *widget->as<ActionableWidget>();
	auto& style = *widget->getStyle<Style>();
	auto& prop = *widget->getProperties<Button::Properties>();

	bool inArea = InputCondition::Mouse::InArea(prop.X, prop.Y, prop.Width, prop.Height, *args);
	
	auto& uniState = UnifiedStateManager::getInstance();
	if (!uniState.MouseInput(args, &actionable, inArea)) return;

	MouseEventArgsEx output;
	output.inArea = inArea;
	output.args = *args;

	if (inArea || uniState.isCaptured()) {
		if (args->buttonState[0]) {
			style.setIndex(2);
			actionable.callEvent(Events::onMouseHeld, args, true);
		}
		else {
			style.setIndex(1);
			actionable.callEvent(Events::onMouseReleased, args, true);
		}
		bsHandler.Arrange(*args);
		if (bsHandler.Timing()) {
			output.pressResult = &bsHandler.getResult();
			actionable.callEvent(Events::onMousePressed, args, true);
		}
		if (args->eventflags[1]) {
			actionable.callEvent(Events::onMouseDoubleClick, args, true);
		}
	}
	else {
		style.setIndex(0);
		bsHandler.Cancel();
	}
	actionable.callEvent(Events::_MouseEventEx_, &output, true);
}

void NoobButton::Style::KeyEvent(KeyEventArgs* args, Widget* widget) {
	auto& actionable = *widget->as<ActionableWidget>();
	if (isButtonSelected) {
		actionable.callEvent(Events::onButtonSelected, args, true);
	}
	if (isSelectionCanceled) { // Executes once
		actionable.callEvent(Events::onButtonSelectionCanceled, args, true);
		isSelectionCanceled = false;
	}
}

void NoobButton::Style::selectButton() {
	isButtonSelected = true;
}

void NoobButton::Style::deselectButton() {
	isButtonSelected = false;
	isSelectionCanceled = true;
}

void NoobButton::SetButtonCaption(std::string caption) {
	getProperties<Properties>()->Caption = caption;
}

void RadioButton::Style::DrawPrimitive(ScreenA& screen, CanvasA& canvas, Widget* widget) {
	auto& prop = *widget->getProperties<Properties>();
	Pixel rectColor = prop.rectBoxColor;
	switch (getIndex()) {
	case 1:
		rectColor.bg.Alpha += 60;
		break;
	case 2:
		rectColor.bg.Alpha -= 20;
		break;
	}

	std::string caption = "";
	if (stateToggle) {
		caption = prop.toggleEnableString;
	}
	else {
		caption = prop.toggleDisableString;
	}

	prop.Width = caption.length();

	canvas.FillRect(prop.X, prop.Y, prop.X + prop.Width, prop.Y + prop.Height, rectColor);
	canvas.DrawString(prop.X, prop.Y, caption, { {},{255,255,255,255} });
}

void RadioButton::Style::setStateToggle(bool flag) {
	stateToggle = flag;
}

bool RadioButton::Style::getStateToggle() {
	return stateToggle;
}

void RadioButton::Init() {
	NoobButton::Init();
	SetStyle(new Style());
	SetDrawer([this](ScreenA* screen, CanvasA* canvas, KeyFrameGroupLoader loader, void* args) {
		style->DrawPrimitive(*screen, *canvas, this);
	});
	addEvent(Events::onMousePressed, [this](void* args, ActionableWidget* widget) {
		auto& style = *widget->getStyle<Style>();
		style.setStateToggle(!style.getStateToggle());
	});
}

void RadioButton::setStateToggle(bool flag) {
	getStyle<Style>()->setStateToggle(flag);
}

bool RadioButton::getStateToggle() {
	return getStyle<Style>()->getStateToggle();
}