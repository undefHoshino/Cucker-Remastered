#include "Overlay.h"

void Overlay::OverlayCreation(ScreenA* main, LogOverlay* logger, Displayer* display, int width, int height, int x, int y) {
	std::shared_ptr<ScreenA> oscreen = std::make_shared<ScreenA>(&main->getBlender());
	std::shared_ptr<CanvasA> ocanvas = std::make_shared<CanvasA>(oscreen.get());

	Creation(oscreen.get(), ocanvas.get(), logger, display);

	dispatcher.attach(main);
	resize(width, height);
	setPosition(x, y);
}
void Overlay::OverlayRender() {
	onCreateScreen();
	Render();
	onWriteConsole();
}
void Overlay::onCreateScreen() {
	screen->resize(overlayWidth, overlayHeight);
}
void Overlay::onWriteConsole() {
	logger->Render(*screen, *canvas);
	dispatcher.SendBufferData(screen->getBuffer(), overlayX, overlayY, border.left, border.right, border.top, border.bottom, attribute.charOverlay, attribute.blend, attribute.Alpha);
}
void Overlay::freeOverlay() {
	if (screen != nullptr) {
		delete screen;
		screen = nullptr;
	}
	if (canvas != nullptr) {
		delete canvas;
		canvas = nullptr;
	}
	logger = nullptr;
	displayer = nullptr;
}
void Overlay::setPosition(int x, int y) {
	overlayX = x;
	overlayY = y;
}
void Overlay::setAttribute(bool wCharOverlay, Pixel::BlendMode wBlend, float wAlpha) {
	this->attribute = { wCharOverlay ,wBlend ,wAlpha };
}
void Overlay::resize(int width, int height) {
	this->overlayWidth = width;
	this->overlayHeight = height;
}