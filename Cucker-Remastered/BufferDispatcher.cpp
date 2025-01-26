#include "BufferDispatcher.h"

void BufferDispatcherA::SendBufferData(ScreenBufferA buf, int offsetX, int offsetY, int borderLeft, int borderRight, int borderTop, int borderBottom, bool charOverlay, Pixel::BlendMode blend, float Alpha) {
	const int bufWidth = buf.Width;
	const int bufHeight = buf.Height;
	const int parentWidth = buffer->Width;  // ��ȡ����Ļ���
	const int parentHeight = buffer->Height; // ��ȡ����Ļ�߶�
	for (int y = 0; y < bufHeight; ++y) {
		int targetY = offsetY + y;

		// ��� Y ���곬������������
		if (targetY < borderTop || targetY >= borderBottom) {
			continue;
		}

		for (int x = 0; x < bufWidth; ++x) {
			int targetX = offsetX + x;

			// ��� X ���곬��������������
			if (targetX < borderLeft || targetX >= borderRight) {
				continue;
			}

			char c = buf.buf[y * bufWidth + x];
			c = buf.buf[y * bufWidth + x] != ' ' && !charOverlay ? c : '\0';
			Pixel pixel = buf.pixels[y * bufWidth + x];
			pixel.mode = blend;
			pixel = pixel * Alpha;
			// ������Ļ������ӳ�䵽����Ļ
			buffer->set(getPos(targetX, targetY), c, pixel);
		}

		if (charOverlay) {
			// ���� String
			std::string lineStr = buf.buf.substr(getPos(0, y, bufWidth), bufWidth);
			if (!stringFix(offsetX, offsetY + y, lineStr)) continue;

			buffer->buf.replace(getPos(offsetX < 0 ? 0 : offsetX, targetY), lineStr.length(), lineStr);
		}
	}
}
bool BufferDispatcherA::stringFix(int x, int y, std::string& str) {
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