#pragma once
#include "Slider.h"

// Unfinished !!!
class Label : public ActionableWidget {
public:
	struct Properties : public RectProperties {
		std::string labelStr;
		Pixel textColor;
		Properties() = default;
		Properties(int x, int y, UINT width, UINT height, std::string str, Pixel textColor = { {},{255,255,255,255} }) :
			RectProperties(x, y, width, height), labelStr(str), textColor(textColor) {};
	};
	class Style : public WidgetStyle {
	public:
		virtual void DrawPrimitive(ScreenA& screen, CanvasA& canvas, Widget* widget) override {
			auto& actionable = *widget->as<ActionableWidget>();
			auto& prop = *widget->getProperties<Properties>();
			auto& style = *widget->getStyle<Style>();
			std::stringstream stream(prop.labelStr);
			std::string line;
			size_t _height = 0;
			while (std::getline(stream, line)) {
				_height++;
				canvas.DrawString(prop.X, prop.Y, line, prop.textColor);
			}

			canvas.FillRect(prop.X, prop.Y, prop.Width, prop.Height, {});
		}
		virtual void MouseEvent(MouseEventArgs* args, Widget* widget) override {

		}
	};
protected:
	Scrollbar horizontalScrollbar;
	Scrollbar verticalScrollbar;
	virtual void Init() override {
		Widget::Init();
		SetStyle(new Style());
		SetDrawer([this](ScreenA* screen, CanvasA* canvas, KeyFrameGroupLoader loader, void* args) {
			style->DrawPrimitive(*screen, *canvas, this);
		});
	}
	void setLabelText(std::string str) {
		getProperties<Properties>()->labelStr = str;
	}
};