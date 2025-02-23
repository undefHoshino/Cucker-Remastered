#pragma once
#include "Widget.h"

class WidgetWrappers {
private:
	std::vector<Widget*> widgets;
public:
	void addWidget(Widget* widget) {
		widgets.push_back(widget);
	}
	
};
