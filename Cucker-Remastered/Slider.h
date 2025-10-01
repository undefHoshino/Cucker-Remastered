#pragma once
#include "WidgetBase.h"
#include "AnimateValue.h"

class Slider : public ActionableWidget {
public:
	enum struct Direction {
		Horizontal,		 // 水平
		HorizontalLeft,  // 水平方向：向左
		HorizontalRight, // 水平方向：向右
		Vertical,		 // 垂直
		VerticalUp,      // 垂直方向：向上
		VerticalDown     // 垂直方向：向下
	};
	struct Data : public WidgetData {
		int sliderValue = 0;
	};
	struct Properties : public RectProperties {
		size_t stepCount = 0;
		Direction direction = Direction::HorizontalRight;
		Pixel rectBoxColor = { {255,255,255,100},{} };
		Properties() = default;
		Properties(int x, int y, UINT width, UINT height, Direction direction = Direction::HorizontalRight, size_t stepCount = 0, Pixel rectBoxColor = { {255,255,255,100},{} }) :
			RectProperties(x, y, width, height), stepCount(stepCount), direction(direction), rectBoxColor(rectBoxColor) {};
	};
	class Style : public WidgetStyle {
	protected:
		ValueUpdater<int> valueUpdater;
	public:
		virtual void DrawPrimitive(ScreenA& screen, CanvasA& canvas, Widget* widget) override;
		virtual void MouseEvent(MouseEventArgs* args, Widget* widget) override;
		bool inAreaJudge(const MouseEventArgs* args, const Properties& prop);
		int getSliderValueForDirection(Direction direction, const MouseEventArgs* args, const Properties& prop);
		ValueUpdater<int>& getValueUpdater();
	};
public:
	// Based on _MouseEvent_
	enum Events {
		onSliderValueChanged = 0,
		onSliderMoving = 1
	};

	virtual void Init() override;
	virtual void setSliderProgeress(float progress);
	virtual float getSliderProgress();
	virtual void setStepCount(size_t stepCount);

	template<class T>
	T lerp(T start, T end) {
		return ((end - start) * getSliderProgress()) + start;
	}
};

class Scrollbar : public Slider {
public:
	struct Properties : public Slider::Properties {
		size_t viewportLength;
		size_t contentLength;
		Properties() = default;
		Properties(int x, int y, UINT width, UINT height, size_t viewportLength, size_t contentLength, Direction direction = Direction::Horizontal, Pixel rectBoxColor = { {255,255,255,100},{} }, size_t stepCountUnused = 0) :
			Slider::Properties(x, y, width, height, direction, stepCountUnused, rectBoxColor), viewportLength(viewportLength), contentLength(contentLength) {};
	};
	class Style : public Slider::Style {
	private:
		int dragOffset;
		bool isDragging = false;
	public:
		virtual void DrawPrimitive(ScreenA& screen, CanvasA& canvas, Widget* widget) override;
		virtual void MouseEvent(MouseEventArgs* args, Widget* widget) override;
		void dragScrollSlider(MouseEventArgs* args, bool inArea, const Properties& prop, Data& data, bool isHorizontal, bool isVertical, ActionableWidget* actionable);
		bool inAreaJudge(const MouseEventArgs* args, const Properties& prop, const Data& data);
		int getSliderValueForDirection(Direction direction, const MouseEventArgs* args, const Properties& prop);
		size_t getSliderSize(const Properties& prop);
	};
public:
	// Based on _MouseEvent_
	enum Events {
		onScrollDragCancel = 5
	};
	virtual void Init() override;
	virtual void setSliderProgeress(float progress) override;
	virtual float getSliderProgress() override;
protected:
	size_t getSliderSize(const Properties& prop);
};