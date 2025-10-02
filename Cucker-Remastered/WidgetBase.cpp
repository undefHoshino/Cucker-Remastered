#include "WidgetBase.h"

void Widget::Render(ScreenA& screen, CanvasA& canvas) {
	animator.DrawUnified(&screen, &canvas, 0);
};

void Widget::SetProperties(WidgetProperties* prop) {
	if (this->properties) delete this->properties;
	this->properties = prop;
}

void Widget::Init() {
	SetData({});
	SetStyle({});
}

void Widget::SetDrawer(TimeAxis::Drawer::DrawFunction func) {
	animator.SetDrawer(nullptr, func);
}

void Widget::SetData(WidgetData* data) {
	if (this->data) delete this->data;
	this->data = data;
}

void Widget::SetStyle(WidgetStyle* style) {
	if (this->style) delete this->style;
	this->style = style;
}

Animator& Widget::getAnimator() {
	return animator;
}

MapContainer& Widget::getContainer() {
	return container;
}

Widget::~Widget() {
	if (properties) {
		delete properties;
		properties = nullptr;
	}
	if (data) {
		delete data;
		data = nullptr;
	}
	if (style) {
		delete style;
		style = nullptr;
	}
}

void ActionableWidget::Mouse(MouseEventArgs args) {
	callEvent(Events::_MouseEvent_, static_cast<void*>(&args));
}

void ActionableWidget::Key(KeyEventArgs args) {
	callEvent(Events::_KeyEvent_, static_cast<void*>(&args));
}

void ActionableWidget::WinFocus(WinFocusEventArgs args) {
	callEvent(Events::_WinFocusEvent_, static_cast<void*>(&args));
}

void ActionableWidget::WinMenu(WinMenuEventArgs args) {
	callEvent(Events::_WinMenuEvent_, static_cast<void*>(&args));
}

void ActionableWidget::WinBufferSize(WinBufferEventArgs args) {
	callEvent(Events::_WinBufferSizeEvent_, static_cast<void*>(&args));
}

void ActionableWidget::Render(ScreenA& screen, CanvasA& canvas) {
	animator.DrawUnified(&screen, &canvas, static_cast<void*>(this));
};

void ActionableWidget::Update() {
	callEvent(Events::_UpdateEvent_, 0);
}

void ActionableWidget::addEvent(int event, _EventFunction func) {
	eventDispatcher.addEvent(event, func);
}

void ActionableWidget::removeEvent(int event) {
	eventDispatcher.removeEvent(event);
}

void ActionableWidget::refreshEvent(int event) {
	eventDispatcher.refreshEvent(event);
}

void ActionableWidget::callEvent(int event, void* args, bool quiet) {
	if (!existEvent(event)) {
		if (quiet) return;
		throw std::runtime_error("Event (called " + std::to_string(event) + ") doesn't exist");
	}
	eventDispatcher.dispatchEvent(event, std::forward<void*>(args), this);
}

bool ActionableWidget::existEvent(int event) {
	return eventDispatcher.existEvent(event);
}

void ActionableWidget::SetWidgetRectPosition(int x, int y) {
	auto& prop = *getProperties<RectProperties>();
	prop.X = x;
	prop.Y = y;
}

void ActionableWidget::SetWidgetRectSize(UINT width, UINT height) {
	auto& prop = *getProperties<RectProperties>();
	prop.Width = width;
	prop.Height = height;
}

MouseCapture& MouseCapture::getInstance() {
	static MouseCapture instance;
	return instance;
}

bool MouseCapture::MouseInput(MouseEventArgs* args, ActionableWidget* widget, bool isInArea) {
	if (!shouldDeliverTo(widget) || backgroundCaptured) return false;

	if (!args->buttonState[0] && isCaptured()) {
		capturedWidget->callEvent(ActionableWidget::Events::_LostCaptureEvent_, 0, true);
		release();
	}

	if (args->buttonState[0] && !capturing && isInArea) {
		capture(widget);
		capturedWidget->callEvent(ActionableWidget::Events::_AcquireCaptureEvent_, 0, true);
	}

	return true;
}

void MouseCapture::MouseBackground(MouseEventArgs* args) {
	if (isCaptured()) return;

	if (args->buttonState[0]) {
		if (!backgroundCaptured) {
			release();
			backgroundCaptured = true;
		}
	}
	else if (backgroundCaptured) {
		backgroundCaptured = false;
	}
}

void MouseCapture::capture(ActionableWidget* widget) {
	release();
	capturing = true;
	capturedWidget = widget;
}

void MouseCapture::release() {
	capturing = false;
	capturedWidget = nullptr;
}

bool MouseCapture::isCaptured() const {
	return capturedWidget != nullptr;
}

ActionableWidget* MouseCapture::getCapturedWidget() const {
	return capturedWidget;
}

bool MouseCapture::shouldDeliverTo(ActionableWidget* widget) const {
	return !isCaptured() || capturedWidget == widget;
}


WidgetFocus& WidgetFocus::getInstance() {
	static WidgetFocus instance;
	return instance;
}

bool WidgetFocus::MouseInput(MouseEventArgs* args, ActionableWidget* widget, bool isInArea) {
	if (args->buttonState[0] && isInArea) {
		setFocus(widget);
		isHitWidget = true;
	}

	return true;
}

void WidgetFocus::MouseBackground(MouseEventArgs* args) {
	if (isHitWidget) {
		isHitWidget = false;
		return;
	}

	if (args->buttonState[0]) {
		if (!backgroundFocus) {
			clearFocus();
			backgroundFocus = true;
		}
	}
	else if (backgroundFocus) {
		backgroundFocus = false;
	}
}

void WidgetFocus::setFocus(ActionableWidget* widget) {
	if (focusedWidget == widget) return;

	if (hasFocusedWidget()) {
		focusedWidget->callEvent(ActionableWidget::Events::_LostFocusEvent_, 0, true);
	}

	focusedWidget = widget;

	if (hasFocusedWidget()) {
		focusedWidget->callEvent(ActionableWidget::Events::_AcquireFocusEvent_, 0, true);
	}
}

void WidgetFocus::clearFocus() {
	setFocus(nullptr);
}

bool WidgetFocus::hasFocusedWidget() {
	return focusedWidget != nullptr;
}

ActionableWidget* WidgetFocus::getFocusedWidget() const {
	return focusedWidget;
}


UnifiedStateManager::UnifiedStateManager() {
	mouseCaptureInst = &MouseCapture::getInstance();
	widgetFocusInst = &WidgetFocus::getInstance();
}
UnifiedStateManager& UnifiedStateManager::getInstance() {
	static UnifiedStateManager instance;
	return instance;
}

bool UnifiedStateManager::MouseInput(MouseEventArgs* args, ActionableWidget* widget, bool isInArea) {
	if (!mouseCaptureInst->MouseInput(args, widget, isInArea)) {
		return false;
	}

	widgetFocusInst->MouseInput(args, widget, isInArea);
	return true;
}

void UnifiedStateManager::MouseBackground(MouseEventArgs* args) {
	mouseCaptureInst->MouseBackground(args);
	if (!mouseCaptureInst->isCaptured()) {
		widgetFocusInst->MouseBackground(args);
	}
}

bool UnifiedStateManager::isCaptured() {
	return mouseCaptureInst->isCaptured();
}

bool UnifiedStateManager::isFocused() {
	return widgetFocusInst->hasFocusedWidget();
}