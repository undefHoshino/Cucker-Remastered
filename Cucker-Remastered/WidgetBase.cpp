#include "WidgetBase.h"

void Widget::Render(ScreenA& screen, CanvasA& canvas) {
	animator.DrawUnified(&screen, &canvas, 0);
};

void Widget::SetProperties(WidgetProperties* prop) {
	if (this->properties) delete this->properties;
	this->properties = prop;
}

void Widget::Init() {
	SetWidget();
}

void Widget::SetWidget(WidgetStyle* style, WidgetData* data) {
	SetStyle(style);
	SetData(data);
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

void ActionableWidget::Focus(FocusEventArgs args) {
	callEvent(Events::_FocusEvent_, static_cast<void*>(&args));
}

void ActionableWidget::Menu(MenuEventArgs args) {
	callEvent(Events::_MenuEvent_, static_cast<void*>(&args));
}

void ActionableWidget::BufferSize(BufferEventArgs args) {
	callEvent(Events::_BufferSizeEvent_, static_cast<void*>(&args));
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