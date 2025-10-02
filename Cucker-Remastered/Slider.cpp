#include "Slider.h"

void Slider::Style::DrawPrimitive(ScreenA& screen, CanvasA& canvas, Widget* widget) {
	auto& prop = *widget->as<Slider>()->getProperties<Properties>();
	auto& data = *widget->as<Slider>()->getData<Data>();

	Pixel topRectColor = prop.rectBoxColor;
	switch (prop.direction) {
	case Direction::HorizontalLeft:
		// 水平方向：从右向左
		canvas.FillRect(prop.X + prop.Width - data.sliderValue, prop.Y, prop.X + prop.Width, prop.Y + prop.Height, topRectColor);
		canvas.FillRect(prop.X, prop.Y, prop.X + prop.Width, prop.Y + prop.Height, prop.rectBoxColor);
		break;
	case Direction::Horizontal:
	case Direction::HorizontalRight:
		// 水平方向：从左向右
		canvas.FillRect(prop.X, prop.Y, prop.X + data.sliderValue, prop.Y + prop.Height, topRectColor);
		canvas.FillRect(prop.X, prop.Y, prop.X + prop.Width, prop.Y + prop.Height, prop.rectBoxColor);
		break;
	case Direction::VerticalUp:
		// 垂直方向：从下向上
		canvas.FillRect(prop.X, prop.Y + prop.Height - data.sliderValue, prop.X + prop.Width, prop.Y + prop.Height, topRectColor);
		canvas.FillRect(prop.X, prop.Y, prop.X + prop.Width, prop.Y + prop.Height, prop.rectBoxColor);
		break;
	case Direction::Vertical:
	case Direction::VerticalDown:
		// 垂直方向：从上向下
		canvas.FillRect(prop.X, prop.Y, prop.X + prop.Width, prop.Y + data.sliderValue, topRectColor);
		canvas.FillRect(prop.X, prop.Y, prop.X + prop.Width, prop.Y + prop.Height, prop.rectBoxColor);
		break;
	}
}

void Slider::Style::MouseEvent(MouseEventArgs* args, Widget* widget) {
	auto& actionable = *widget->as<ActionableWidget>();
	auto& prop = *widget->getProperties<Properties>();
	auto& data = *widget->getData<Data>();
	auto& style = *widget->getStyle<Style>();
	bool inArea = inAreaJudge(args, prop);

	auto& uniState = UnifiedStateManager::getInstance();
	if (!uniState.MouseInput(args, &actionable, inArea)) return;

	if ((inArea && args->buttonState[0]) || uniState.isCaptured()) {
		data.sliderValue = getSliderValueForDirection(prop.direction, args, prop);

		actionable.callEvent(Events::onSliderMoving, args, true);
		if (style.valueUpdater.update(data.sliderValue))
			actionable.callEvent(Events::onSliderValueChanged, args, true);
	}
};

bool Slider::Style::inAreaJudge(const MouseEventArgs* args, const Properties& prop) {
	auto judgeX = prop.X;
	auto judgeY = prop.Y;
	int64_t judgeWidth = prop.Width;
	int64_t judgeHeight = prop.Height;
	switch (prop.direction) {
	case Direction::Horizontal:
	case Direction::HorizontalLeft:
	case Direction::HorizontalRight:
		judgeWidth++;
		break;
	case Direction::Vertical:
	case Direction::VerticalUp:
	case Direction::VerticalDown:
		judgeHeight++;
		break;
	}
	return InputCondition::Mouse::InArea(judgeX, judgeY, judgeWidth, judgeHeight, *args);
}

int Slider::Style::getSliderValueForDirection(Direction direction, const MouseEventArgs* args, const Properties& prop) {
	int sliderValue = 0;
	switch (direction) {
	case Direction::HorizontalLeft:
		sliderValue = std::min(std::max(0, (int)(prop.X + prop.Width - args->position.X)), (int)prop.Width);
		break;
	case Direction::Horizontal:
	case Direction::HorizontalRight:
		sliderValue = std::min(std::max(0, args->position.X - prop.X), (int)prop.Width);
		break;
	case Direction::VerticalUp:
		sliderValue = std::min(std::max(0, (int)(prop.Y + prop.Height - args->position.Y)), (int)prop.Height);
		break;
	case Direction::Vertical:
	case Direction::VerticalDown:
		sliderValue = std::min(std::max(0, args->position.Y - prop.Y), (int)prop.Height);
		break;
	}

	// 支持 stepCount 的处理
	if (prop.stepCount > 0) {
		size_t sliderLength = (direction == Direction::Horizontal || direction == Direction::HorizontalLeft || direction == Direction::HorizontalRight)
			? prop.Width
			: prop.Height;
		int stepSize = sliderLength / (prop.stepCount - 1);
		int maxStep = stepSize * (prop.stepCount - 1);  // 累积的最大步进
		if (sliderValue > maxStep) {
			sliderValue = sliderLength;
		}
		else {
			// 四舍五入到最接近的步长
			sliderValue = std::round((float)sliderValue / stepSize) * stepSize;
		}
	}

	return sliderValue;
}

ValueUpdater<int>& Slider::Style::getValueUpdater() {
	return valueUpdater;
}

void Slider::Init() {
	Widget::Init();
	SetStyle(new Style());
	SetData(new Data());
	SetDrawer([this](ScreenA* screen, CanvasA* canvas, KeyFrameGroupLoader loader, void* args) {
		style->DrawPrimitive(*screen, *canvas, this);
		});
	getStyle<Style>()->getValueUpdater().initial(0);
	addEvent(Slider::_MouseEvent_, [this](void* _args, ActionableWidget* self) { style->MouseEvent(static_cast<MouseEventArgs*>(_args), self); });
}

void Slider::setSliderProgeress(float progress) {
	progress = std::clamp(progress, 0.0f, 1.0f);
	auto& prop = *getProperties<Properties>();
	auto& data = *getData<Data>();

	if (prop.stepCount > 0) {
		float stepSize = 1.0f / prop.stepCount; // 每阶段对应的进度比例
		progress = std::round(progress / stepSize) * stepSize;
	}

	switch (prop.direction) {
	case Direction::Horizontal:
	case Direction::HorizontalLeft:
	case Direction::HorizontalRight: {
		data.sliderValue = std::round(progress * prop.Width);
		break;
	}
	case Direction::Vertical:
	case Direction::VerticalUp:
	case Direction::VerticalDown: {
		data.sliderValue = std::round(progress * prop.Height);
		break;
	}
	}
}

float Slider::getSliderProgress() {
	auto& prop = *getProperties<Properties>();
	switch (prop.direction) {
	case Direction::Horizontal:
	case Direction::HorizontalLeft:
	case Direction::HorizontalRight: {
		if (prop.Width == 0) return 0.0;
		return (double)getData<Data>()->sliderValue / (double)prop.Width;
	}
	case Direction::Vertical:
	case Direction::VerticalUp:
	case Direction::VerticalDown: {
		if (prop.Height == 0) return 0.0;
		return (double)getData<Data>()->sliderValue / (double)prop.Height;
	}
	}
	return 0.0f;
}

void Slider::setStepCount(size_t stepCount) {
	getProperties<Properties>()->stepCount = stepCount;
}

void Scrollbar::Style::DrawPrimitive(ScreenA& screen, CanvasA& canvas, Widget* widget) {
	auto& prop = *widget->as<Scrollbar>()->getProperties<Properties>();
	auto& data = *widget->as<Scrollbar>()->getData<Data>();

	Pixel topRectColor = prop.rectBoxColor;
	size_t sliderSize = getSliderSize(prop);
	switch (prop.direction) {
	case Direction::Horizontal:
	case Direction::HorizontalLeft:
	case Direction::HorizontalRight: {
		// 水平
		int64_t sliderPosX = prop.X + data.sliderValue;
		canvas.FillRect(prop.X + data.sliderValue, prop.Y, sliderPosX + sliderSize, prop.Y + prop.Height, topRectColor);
		canvas.FillRect(prop.X, prop.Y, prop.X + prop.Width, prop.Y + prop.Height, prop.rectBoxColor);
		break;
	}
	case Direction::Vertical:
	case Direction::VerticalUp:
	case Direction::VerticalDown: {
		// 垂直
		int64_t sliderPosY = prop.Y + data.sliderValue;
		canvas.FillRect(prop.X, sliderPosY, prop.X + prop.Width, sliderPosY + sliderSize, topRectColor);
		canvas.FillRect(prop.X, prop.Y, prop.X + prop.Width, prop.Y + prop.Height, prop.rectBoxColor);
		break;
	}
	}
}

void Scrollbar::Style::MouseEvent(MouseEventArgs* args, Widget* widget) {
	auto& actionable = *widget->as<ActionableWidget>();
	auto& prop = *widget->getProperties<Properties>();
	auto& data = *widget->getData<Data>();
	auto& style = *widget->getStyle<Style>();

	bool inArea = inAreaJudge(args, prop, data);
	auto& uniState = UnifiedStateManager::getInstance();
	if (!uniState.MouseInput(args, &actionable, inArea)) return;

	int mousePos = 0;
	bool isHorizontal = false;
	bool isVertical = false;
	switch (prop.direction) {
	case Direction::Horizontal:
	case Direction::HorizontalLeft:
	case Direction::HorizontalRight:
		isHorizontal = true;
		break;

	case Direction::Vertical:
	case Direction::VerticalUp:
	case Direction::VerticalDown:
		isVertical = true;
		break;
	}

	dragScrollSlider(args, inArea, prop, data, isHorizontal, isVertical, &actionable);

	if (inArea || isDragging || uniState.isCaptured()) {
		actionable.callEvent(Slider::Events::onSliderMoving, args, true);
		if (style.valueUpdater.update(data.sliderValue))
			actionable.callEvent(Slider::Events::onSliderValueChanged, args, true);
	}
	
}

void Scrollbar::Style::dragScrollSlider(MouseEventArgs* args, bool inArea, const Properties& prop, Data& data, bool isHorizontal, bool isVertical, ActionableWidget* actionable) {
	if (!inArea && !isDragging) return;
	size_t sliderSize = getSliderSize(prop);

	if (args->buttonState[0]) { // 鼠标按下状态
		if (!isDragging) {
			// 初始化偏移量，只执行一次
			dragOffset = (isHorizontal ? args->position.X : args->position.Y) - data.sliderValue;
			isDragging = true;
		}

		// 根据鼠标位置和偏移量更新滑块值
		int mousePos = isHorizontal ? args->position.X : args->position.Y;
		data.sliderValue = mousePos - dragOffset;

		// 限制滑块的范围
		int maxSliderValue = isHorizontal ? (int)(prop.Width - sliderSize) : (int)(prop.Height - sliderSize);
		data.sliderValue = std::clamp(data.sliderValue, 0, maxSliderValue);
	}
	else {
		// 鼠标松开，结束拖动
		isDragging = false;
		actionable->callEvent(Events::onScrollDragCancel, args, true);
	}
}

bool Scrollbar::Style::inAreaJudge(const MouseEventArgs* args, const Properties& prop, const Data& data) {
	auto judgeX = prop.X;
	auto judgeY = prop.Y;
	int64_t judgeWidth = prop.Width;
	int64_t judgeHeight = prop.Height;
	size_t sliderSize = getSliderSize(prop);
	switch (prop.direction) {
	case Direction::Horizontal:
	case Direction::HorizontalLeft:
	case Direction::HorizontalRight:
		judgeX = prop.X + data.sliderValue;
		judgeWidth = sliderSize;
		break;
	case Direction::Vertical:
	case Direction::VerticalUp:
	case Direction::VerticalDown:
		judgeY = prop.Y + data.sliderValue;
		judgeHeight = sliderSize;
		break;
	}
	return InputCondition::Mouse::InArea(judgeX, judgeY, judgeWidth, judgeHeight, *args);
}

int Scrollbar::Style::getSliderValueForDirection(Direction direction, const MouseEventArgs* args, const Properties& prop) {
	switch (direction) {
	case Direction::Horizontal:
	case Direction::HorizontalLeft:
	case Direction::HorizontalRight:
		return std::min(std::max(0, args->position.X - prop.X), (int)prop.Width);
	case Direction::Vertical:
	case Direction::VerticalUp:
	case Direction::VerticalDown:
		return std::min(std::max(0, args->position.Y - prop.Y), (int)prop.Height);
	}
	return 0.0;
}

size_t Scrollbar::Style::getSliderSize(const Properties& prop) {
	return std::max(prop.viewportLength * (prop.viewportLength / prop.contentLength), (uint64_t)2);
}

void Scrollbar::Init() {
	Widget::Init();
	SetStyle(new Style());
	SetData(new Data());
	SetDrawer([this](ScreenA* screen, CanvasA* canvas, KeyFrameGroupLoader loader, void* args) {
		style->DrawPrimitive(*screen, *canvas, this);
		});
	getStyle<Style>()->getValueUpdater().initial(0);
	addEvent(Slider::_MouseEvent_, [this](void* _args, ActionableWidget* self) { style->MouseEvent(static_cast<MouseEventArgs*>(_args), self); });
}

void Scrollbar::setSliderProgeress(float progress) {
	progress = std::clamp(progress, 0.0f, 1.0f);
	auto& prop = *getProperties<Properties>();
	auto& data = *getData<Data>();
	size_t size = getSliderSize(prop);
	switch (prop.direction) {
	case Direction::Horizontal:
	case Direction::HorizontalLeft:
	case Direction::HorizontalRight: {
		data.sliderValue = std::round(progress * (prop.Width - size));
		break;
	}
	case Direction::Vertical:
	case Direction::VerticalUp:
	case Direction::VerticalDown: {
		data.sliderValue = std::round(progress * (prop.Height - size));
		break;
	}
	}
}

float Scrollbar::getSliderProgress() {
	auto& prop = *getProperties<Properties>();
	auto& data = *getData<Data>();
	auto size = getSliderSize(prop);
	switch (prop.direction) {
	case Direction::Horizontal:
	case Direction::HorizontalLeft:
	case Direction::HorizontalRight: {
		if ((int64_t)prop.Width - size <= 0) return 0.0;
		return (double)data.sliderValue / (double)(prop.Width - size);
	}
	case Direction::Vertical:
	case Direction::VerticalUp:
	case Direction::VerticalDown: {
		if ((int64_t)prop.Height - size <= 0) return 0.0;
		return (double)data.sliderValue / (double)(prop.Height - size);
	}
	}
	return 0.0f;
}

size_t Scrollbar::getSliderSize(const Properties& prop) {
	return std::max(prop.viewportLength * (prop.viewportLength / prop.contentLength), (uint64_t)2);
}