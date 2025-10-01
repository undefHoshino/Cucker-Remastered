#pragma once
#include <string>
#include <vector>
#include "Chroma.h"
#undef min
#undef max

template<class String>
class IScreenBuffer {
public:
	int Width, Height;

	virtual void blend(size_t index, Pixel p) = 0;
	virtual void set(size_t index, char c, Pixel pixel) = 0;
	virtual void assign(size_t index, size_t amount, char c, Pixel pixel, bool overflow = false, size_t x = 0) = 0;
	virtual void replace(size_t index, std::string str, Pixel pixel) = 0;
	virtual void clear(size_t capacity) = 0;
	virtual void shrink() = 0;
	virtual void free() = 0;
};


class ScreenBufferA : public IScreenBuffer<std::string> {
public:
	std::string buf;
	std::vector<Pixel> pixels;

	void blend(size_t index, Pixel p) override;
    void set(size_t index, char c, Pixel pixel) override;
	void assign(size_t index, size_t amount, char c, Pixel pixel, bool overflow = false, size_t x = 0) override;
	void replace(size_t index, std::string str, Pixel pixel) override;
	void clear(size_t capacity) override;
	void shrink() override;
	void free() override;
};
