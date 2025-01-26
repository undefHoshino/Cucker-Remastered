#pragma once
#include <vector>
#include "Point.h"
class PosCalculator {
public:
	enum Align {
		Align_Left,
		Align_Right,
		Align_Top,
		Align_Bottom,
		Align_Center,
		Align_CenterH,
		Align_CenterV
	};

	struct Rect {
		int x = 0; // origin X
		int y = 0; // origin Y
		int width = 0;
		int height = 0;
	};
private:
	Rect rect = {};
public:
	PosCalculator() = default;
	PosCalculator(Rect rect) :rect(rect) {};
	void setRect(Rect rect);
	// X
	int alignLeft();
	// X
	int alignRight(int width);
	// Y
	int alignTop();
	// Y
	int alignBottom();
	// X
	int alignCenterH(int width);
	// Y
	int alignCenterV(int height = 1);
	// X,Y
	PointI alignCenter(int width,int height = 1);
};