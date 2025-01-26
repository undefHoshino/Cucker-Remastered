#include "PosCalculator.h"

void PosCalculator::setRect(Rect rect) {
	this->rect = rect;
}

// X
int PosCalculator::alignLeft() {
	return rect.x;
}
// X
int PosCalculator::alignRight(int width) {
	return rect.x + rect.width - width;
}
// Y
int PosCalculator::alignTop() {
	return rect.y;
}
// Y
int PosCalculator::alignBottom() {
	return rect.y + rect.height;
}
// X
int PosCalculator::alignCenterH(int width) {
	if (width > rect.width) return 0;
	bool elementParity, containerParity;
	int temp;
	elementParity = (width % 2 == 0) ? true : false;
	containerParity = (rect.width % 2 == 0) ? true : false;
	// 4 types
	switch (
		elementParity ?
		(containerParity ? 1 : 2) : (containerParity ? 3 : 4)
		) {
	case 1: // O-O
	case 4: // E-E
		temp = rect.x + (rect.width - width) / 2;
		return (temp > 0) ? temp : 0;
	case 2: // O-E
	case 3: // E-O
		temp = rect.x + ((rect.width - 1) - width) / 2;
		return (temp > 0) ? temp : 0;
	}
	return 0;
}
// Y
int PosCalculator::alignCenterV(int height) {
	if (height > rect.height) return 0;
	bool elementParity, containerParity;
	int temp;
	elementParity = (height % 2 == 0) ? true : false;
	containerParity = (rect.height % 2 == 0) ? true : false;
	// 4 types
	switch (
		elementParity ?
		(containerParity ? 1 : 2) : (containerParity ? 3 : 4)
		) {
	case 1: // O-O
	case 4: // E-E
		temp = rect.y + (rect.height - height) / 2;
		return (temp > 0) ? temp : 0;
	case 2: // O-E
	case 3: // E-O
		temp = rect.y + ((rect.height - 1) - height) / 2;
		return (temp > 0) ? temp : 0;
	}
	return 0;
}
// X,Y
PointI PosCalculator::alignCenter(int width, int height) {
	return { alignCenterH(width), alignCenterV(height) };
}