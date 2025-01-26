#include "FpsOverlay.h"

Pixel FpsOverlay::colors[3] = {
   {{},{165,255,165,255}},     //Green
   {{},{255,255,128,255}},     //Yellow
   {{},{255,140,140,255}}      //Red
};

void FpsOverlay::Render(ScreenA& screen, CanvasA& canvas) {
	if (!data.ready) {
		data.ready = true;
		tickTimer.begin();
		return;
	}
	data.delay = tickTimer.elapsed_micro() / 1000.0f;

	std::string fps = "FPS:" + std::to_string(static_cast<int>(1000.0f / data.delay));
	std::string delay = std::to_string(data.delay).substr(0, 6) + "ms";

	PosCalculator pCalc({ 0,0,screen.getWidth(),screen.getHeight() });

	PointI fpsPos = { pCalc.alignRight((int)fps.length()) - 2 , screen.getHeight() - 3 };
	PointI delayPos = { pCalc.alignRight((int)delay.length()) - 2, screen.getHeight() - 2 };

	canvas.DrawString(fpsPos.x, fpsPos.y, fps, { {},{165,255,165,255} });
	canvas.DrawString(delayPos.x, delayPos.y, delay, { {},{165,255,165,255} });

	tickTimer.begin();
}