#pragma once
#include "Chroma.h"
#include "Timer.h"
#include "Layer.h"
#include "PosCalculator.h"
class FpsOverlay : protected Layer {
private:
	static Pixel colors[3];
	Timer tickTimer;
	
	struct {
		double delay = 0;
		bool ready = false;
	} data;
	
public:
	FpsOverlay() {
		tickTimer.begin();
	}
	void Render(ScreenA& screen, CanvasA& canvas) override;
};