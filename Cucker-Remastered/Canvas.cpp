#include "Canvas.h"

bool CanvasA::stringFix(int &x, int &y, std::string& str) {
	if (x > buffer->Width || y >= buffer->Height || y < 0) {
		return false;
	}
	if (x < 0) {
		size_t absX = static_cast<size_t>(std::abs(x));
		if (absX < str.length()) {
			str = str.substr(absX);
		}
		else {
			return false;
		}
		x = 0;
	}
	if (x + str.length() > buffer->Width) { // overflow
		str = str.substr(0, std::min(str.length(), static_cast<size_t>(buffer->Width) - x));
	}
	return true;
}
void CanvasA::pointIFix(PointI& a, PointI& b) {
	if (a.x > b.x) std::swap(a.x, b.x);
	if (a.y > b.y) std::swap(a.y, b.y);
}

void CanvasA::SetPixel(int x, int y, Pixel pixel, char c) {
	if (x < 0 || y < 0 || !checkPos(x, y)) return;
	buffer->set(getPos(x, y), c, pixel);
}
void CanvasA::DrawString(int x, int y, std::string str, Pixel pixel) {
	if (!stringFix(x, y, str)) return;
	size_t index = getPos(x, y);
	buffer->replace(index, str, pixel);
}
void CanvasA::DrawLineV(int x, int y1, int y2, Pixel pixel, char c, bool ignore) {
	PointI a = { x,y1 };
	PointI b = { x,y2 };
	if (!ignore) {
		a.legal(0, 0, buffer->Width, buffer->Height);
		b.legal(0, 0, buffer->Width, buffer->Height);
		pointIFix(a, b);
	}
	for (USHORT y = a.y; y < b.y; y++) {
		SetPixel(x, y, pixel, c);
	}
}
void CanvasA::DrawLineH(int x1, int x2, int y, Pixel pixel, char c, bool ignore) {
	if (y >= buffer->Height || y < 0) return;

	PointI a = { x1,y };
	PointI b = { x2,y };
	if (!ignore) {
		a.legal(0, 0, buffer->Width, buffer->Height);
		b.legal(0, 0, buffer->Width, buffer->Height);
		pointIFix(a, b);
	}
	buffer->assign(getPos(a.x, a.y), (b.x - a.x), c, pixel, false);
}
void CanvasA::DrawLine(int _x1, int _y1, int _x2, int _y2, Pixel pixel, char c) {
	PointI a = { _x1,_y1 };
	PointI b = { _x2,_y2 };
	a.legal(0, 0, buffer->Width, buffer->Height);
	b.legal(0, 0, buffer->Width, buffer->Height);
	pointIFix(a, b);

	int& x1 = a.x;
	int& x2 = b.x;
	int& y1 = a.y;
	int& y2 = b.y;

	int xMax = b.x;
	int yMax = b.y;
	int dx, dy, sx, sy, err;
	dx = abs(x2 - x1);
	dy = abs(y2 - y1);
	sx = (x1 < x2) ? 1 : -1;
	sy = (y1 < y2) ? 1 : -1;
	err = dx - dy;

	while (true) {
		// 在当前位置设置像素
		if (x1 > xMax || y1 > yMax) break;
		SetPixel(x1, y1, pixel, c);

		// 如果已经到达终点，退出循环
		if (x1 == x2 && y1 == y2) {
			break;
		}
		// 调整x方向
		if (err * 2 > -dy) {
			err -= dy;
			x1 += sx;
		}

		// 调整y方向
		if (err * 2 < dx) {
			err += dx;
			y1 += sy;
		}
	}
}
void CanvasA::FillRect(int x1, int y1, int x2, int y2, Pixel pixel, char c) {
	PointI a = { x1,y1 };
	PointI b = { x2,y2 };
	a.legal(0, 0, buffer->Width, buffer->Height);
	b.legal(0, 0, buffer->Width, buffer->Height);
	pointIFix(a, b);

	size_t lenY = static_cast<size_t>(b.y - a.y);
	for (int y = 0; y < lenY; y++) {
		DrawLineH(a.x, b.x, y + a.y, pixel, c, true);
	}
}
void CanvasA::FillPolygon(std::vector<PointI> vertices, Pixel pixel, char c) {
	int minY = vertices[0].y, maxY = vertices[0].y;
	for (const auto& vertex : vertices) {
		minY = std::min(minY, vertex.y);
		maxY = std::max(maxY, vertex.y);
	}

	// 遍历每条扫描线
	for (int y = minY; y <= maxY; y++) {
		std::vector<USHORT> intersections;

		// 计算扫描线与多边形边的交点
		for (size_t i = 0; i < vertices.size(); i++) {
			PointI p1 = vertices[i], p2 = vertices[(i + 1) % vertices.size()];
			if ((p1.y <= y && p2.y > y) || (p2.y <= y && p1.y > y)) {
				if (p1.y == p2.y) continue;
				USHORT intersectX = p1.x + (y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
				intersections.push_back(intersectX);
			}
		}

		if (intersections.size() == 0) continue;
		// 对交点进行排序
		std::sort(intersections.begin(), intersections.end());

		// 填充像素
		for (size_t i = 0; i < intersections.size(); i += 2) {
			DrawLineV(intersections[i], intersections[i + 1], y, pixel, c);
		}
	}
}