#include "ScreenBuffer.h"

void ScreenBufferA::set(size_t index, char c, Pixel pixel) {
    blend(index, pixel);
    if(c != '\0')
        buf[index] = c;
}
void ScreenBufferA::blend(size_t index, Pixel pixel) {
    switch (pixel.mode) {
    case Pixel::BlendMode::Replace:
        pixels[index] = pixel;
        break;
    case Pixel::BlendMode::Mix:
        pixels[index].bg = Color::Blend(pixels[index].bg, pixel.bg);
        pixels[index].fg = Color::Blend(pixels[index].fg, pixel.fg);
        break;
    case Pixel::BlendMode::Discard:
    default:
        break;
    }
}
void ScreenBufferA::assign(size_t index, size_t amount, char c, Pixel pixel, bool overflow, size_t x) {
	size_t endIndex = overflow ? index + std::min(amount, this->Width - x) : index + amount;
	for (size_t i = index; i < endIndex; i++) {
		blend(i, pixel);
	}
    if (c != '\0')
        buf.replace(index, endIndex - index, endIndex - index, c);
}
void ScreenBufferA::replace(size_t index, std::string str, Pixel pixel) {
    assign(index, str.length(), 0, pixel, false);
    buf.replace(index, str.length(), str);
}
void ScreenBufferA::clear(size_t capacity) {
	pixels.assign(capacity, {});
	buf.assign(capacity, ' ');
}
void ScreenBufferA::shrink() {
	pixels.shrink_to_fit();
	buf.shrink_to_fit();
}
void ScreenBufferA::free() {
	pixels.clear();
	buf.clear();
	shrink();
}