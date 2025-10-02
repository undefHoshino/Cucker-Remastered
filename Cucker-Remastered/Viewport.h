#pragma once
#include "WidgetBase.h"
#include "Slider.h"
#include "BufferDispatcher.h"

// Unfinished !!!
//class ViewportBufferDispatcher : AbstractCanvas<ScreenBufferA> {
//public:
//	ViewportBufferDispatcher() = default;
//	ViewportBufferDispatcher(AbstractScreen<ScreenBufferA>* parent) :AbstractCanvas(parent) {};
//
//	bool Clipper(int viewportX, int viewportY, UINT viewportWidth, UINT viewportHeight,int& clippedWidth,int& clippedHeight) {
//		if (viewportX > buffer->Width) return false;
//		if (viewportY > buffer->Height) return false;
//		if (viewportX < 0) {
//			clippedWidth = std::max(0, (int)viewportWidth + viewportX); // 扣除左边界部分
//			viewportX = 0; // 将视口 X 坐标调整为 0
//		}
//		else {
//			if (viewportX + viewportWidth > buffer->Width) {
//				int overflowWidth = (viewportX + viewportWidth) - buffer->Width;
//				clippedWidth = std::max(0, (int)viewportWidth - overflowWidth);
//			}
//		}
//		if (viewportY < 0) {
//			clippedHeight = std::max(0, (int)viewportHeight + viewportY); // 扣除上边界部分
//			viewportY = 0; // 将视口 Y 坐标调整为 0
//		}
//		else {
//			if (viewportY + viewportHeight > buffer->Height) {
//				int overflowHeight = (viewportY + viewportHeight) - buffer->Height;
//				clippedHeight = std::max(0, (int)viewportHeight - overflowHeight);
//			}
//		}
//	}
//
//	void SendBufferDataViewport(ScreenBufferA buf, int viewportX, int viewportY, UINT viewportWidth, UINT viewportHeight,
//		int contentX, int contentY, UINT contentWidth, UINT contentHeight,
//		bool charOverlay = true, Pixel::BlendMode blend = Pixel::Replace, float Alpha = 1.0f) {
//
//		int bufWidth = viewportWidth;
//		int bufHeight = viewportHeight;
//
//		if (!Clipper(viewportX, viewportY, viewportWidth, viewportHeight, bufWidth, bufHeight)) return;
//		
//		for (int y = 0; y < bufHeight; ++y) {
//			int targetY = viewportX + y;
//
//			// 如果 Y 坐标超界则跳过该行
//			if (targetY < contentX || targetY >= borderBottom) {
//				continue;
//			}
//
//			for (int x = 0; x < bufWidth; ++x) {
//				int targetX = offsetX + x;
//
//				// 如果 X 坐标超界则跳过该像素
//				if (targetX < borderLeft || targetX >= borderRight) {
//					continue;
//				}
//
//				char c = buf.buf[y * bufWidth + x];
//				c = buf.buf[y * bufWidth + x] != ' ' && !charOverlay ? c : '\0';
//				Pixel pixel = buf.pixels[y * bufWidth + x];
//				pixel.mode = blend;
//				pixel = pixel * Alpha;
//				// 将子屏幕的像素映射到主屏幕
//				buffer->set(getPos(targetX, targetY), c, pixel);
//			}
//
//			if (charOverlay) {
//				// 覆盖 String
//				std::string lineStr = buf.buf.substr(getPos(0, y, bufWidth), bufWidth);
//				if (!stringFix(offsetX, offsetY + y, lineStr)) continue;
//
//				buffer->buf.replace(getPos(offsetX < 0 ? 0 : offsetX, targetY), lineStr.length(), lineStr);
//			}
//		}
//	}
//
//	void SendBufferDataContent(ScreenBufferA buf, int viewportX, int viewportY, UINT viewportWidth, UINT viewportHeight,
//		int contentX, int contentY, UINT contentWidth, UINT contentHeight,
//		bool charOverlay = true, Pixel::BlendMode blend = Pixel::Replace, float Alpha = 1.0f) {
//
//	}
//protected:
//	bool stringFix(int x, int y, std::string& str);
//};

//class Display;


// Unfinished !!!
class Viewport : public ActionableWidget {
public:
	struct Rect {
		int x;
		int y;
		UINT width;
		UINT height;
	};
	enum struct ScrollMode {
		MoveContent, // 移动content
		MoveViewport // 移动viewport
	};
	enum struct Direction {
		Vertical,		// 垂直
		Horizontal		// 水平
	};
	struct Properties : public WidgetProperties {
		Rect viewportRect = {};
		Rect contentRect = {};
		ScrollMode moveMode;
		float multiplyAlpha = 1.0f;
		bool charOnly = false;
		Pixel::BlendMode blendMode = Pixel::BlendMode::Replace;

		Properties() = default;
		Properties(Rect viewport, Rect contentRect, ScrollMode moveMode = ScrollMode::MoveViewport, float multiplyAlpha = 1.0f, bool charOnly = false, Pixel::BlendMode blendMode = Pixel::BlendMode::Replace)
			: viewportRect(viewport), contentRect(contentRect), moveMode(moveMode), multiplyAlpha(multiplyAlpha), charOnly(charOnly), blendMode(blendMode) {
			if (contentRect.width < viewportRect.width || contentRect.height < viewportRect.height) {
				Logger("Viewport::Properties").warn("content.size is smaller than viewport.size (Content[",
					contentRect.width, " , ", contentRect.height, "], Viewport[",
					viewportRect.width, " , ", viewportRect.height, "])");
			}
		};
	};
	class Style : public WidgetStyle {
	protected:
		bool dragging = false;
		int lastMousePosX = 0;
		int lastMousePosY = 0;
		bool scrollingInProgress = false;
	public:
		virtual void DrawPrimitive(ScreenA& screen, CanvasA& canvas, Widget* widget) override {
			Viewport* viewport = widget->as<Viewport>();
			auto& prop = *viewport->getProperties<Viewport::Properties>();
			canvas.FillRect(prop.viewportRect.x, prop.viewportRect.y, prop.viewportRect.x + prop.viewportRect.width, prop.viewportRect.y + prop.viewportRect.height, { {255,255,255,100},{} });
			viewport->horizontalScrollbar.Render(screen, canvas);
			viewport->verticalScrollbar.Render(screen, canvas);
		}
		virtual void MouseEvent(MouseEventArgs* args, Widget* widget) override {
			Viewport* viewport = widget->as<Viewport>();
			//ObjectBridge = viewport;
			//LogOverlay::GetInstance().info("MouseEvent - ObjectBridge: ", ObjectBridge);
			viewport->horizontalScrollbar.Mouse(*args);
			viewport->verticalScrollbar.Mouse(*args);

			if (scrollingInProgress) return;

			auto& prop = *viewport->getProperties<Viewport::Properties>();
			if (inAreaJudge(args,prop,viewport)) {
				onDragging(args, prop, viewport);
				scrollMove(args, viewport);
			}
		}
		virtual void KeyEvent(KeyEventArgs* args, Widget* widget) override {

		}
		virtual bool inAreaJudge(const MouseEventArgs* args, const Properties& prop, Viewport* viewport) {
			auto& hscrollProp = *viewport->horizontalScrollbar.getProperties<Scrollbar::Properties>();
			auto& vscrollProp = *viewport->verticalScrollbar.getProperties<Scrollbar::Properties>();
			int judgeX, judgeY;
			UINT judgeWidth, judgeHeight;
			if (prop.moveMode == ScrollMode::MoveContent) {
				judgeX = prop.viewportRect.x;
				judgeY = prop.viewportRect.y;
				judgeWidth = prop.viewportRect.x + prop.viewportRect.width;
				judgeHeight = prop.viewportRect.y + prop.viewportRect.height;
			}
			else { // MoveMode::Viewport
				judgeX = prop.contentRect.x;
				judgeY = prop.contentRect.y;
				judgeWidth = prop.contentRect.x + prop.contentRect.width;
				judgeHeight = prop.contentRect.y + prop.contentRect.height;
			}
			return InputCondition::Mouse::InArea(judgeX, judgeY, judgeWidth, judgeHeight, *args);
		}
		virtual void onDragging(MouseEventArgs* args, Properties& prop, Viewport* viewport) {
			if (args->buttonState[0]) {
				if (!dragging) {
					lastMousePosX = args->position.X;
					lastMousePosY = args->position.Y;
					dragging = true;
				} else {
					// 计算拖动的偏移量
					int dx = args->position.X - lastMousePosX;
					int dy = args->position.Y - lastMousePosY;

					int maxX = prop.contentRect.width - prop.viewportRect.width;
					int maxY = prop.contentRect.height - prop.viewportRect.height;
					// 根据当前的移动模式更新视口或内容
					if (prop.moveMode == ScrollMode::MoveContent) {

						prop.contentRect.x = std::clamp(prop.contentRect.x + dx, 0, maxX);
						prop.contentRect.y = std::clamp(prop.contentRect.y + dy, 0, maxY);
						viewport->horizontalScrollbar.setSliderProgeress((double)prop.contentRect.x / (double)maxX);
						viewport->verticalScrollbar.setSliderProgeress((double)prop.contentRect.y / (double)maxY);
					}
					else { // MoveMode::Viewport
						prop.viewportRect.x = std::clamp(prop.viewportRect.x + dx, 0, maxX);
						prop.viewportRect.y = std::clamp(prop.viewportRect.y + dy, 0, maxY);
						viewport->horizontalScrollbar.setSliderProgeress((double)prop.viewportRect.x / (double)maxX);
						viewport->verticalScrollbar.setSliderProgeress((double)prop.viewportRect.y / (double)maxY);
					}
					lastMousePosX = args->position.X;
					lastMousePosY = args->position.Y;
				}
			}
			else {
				dragging = false;
			}
		}

		virtual void scrollByScrollbar(Scrollbar* scrollBar, Direction direction,Viewport* viewport) {
			auto& prop = *viewport->getProperties<Viewport::Properties>();
			int maxX = prop.contentRect.width - prop.viewportRect.width;
			int maxY = prop.contentRect.height - prop.viewportRect.height;
			switch (direction) {
			case Direction::Horizontal: {
				if (prop.moveMode == ScrollMode::MoveContent) {
					// 移动内容：根据滚动条的进度更新内容位置
					int newX = static_cast<int>(scrollBar->getSliderProgress() * maxX);
					prop.contentRect.x = std::clamp(newX, 0, maxX);
				}
				else { // Viewport
					// 移动视口：根据滚动条的进度更新视口位置
					int newX = static_cast<int>(scrollBar->getSliderProgress() * maxX);
					prop.viewportRect.x = std::clamp(newX, 0, maxX);
				}
				break;
			}
			case Direction::Vertical: {
				if (prop.moveMode == ScrollMode::MoveContent) {
					// 移动内容：根据滚动条的进度更新内容位置
					int newY = static_cast<int>(scrollBar->getSliderProgress() * maxY);
					prop.contentRect.y = std::clamp(newY, 0, maxY);
				}
				else { // Viewport
					// 移动视口：根据滚动条的进度更新视口位置
					int newY = static_cast<int>(scrollBar->getSliderProgress() * maxY);
					prop.viewportRect.y = std::clamp(newY, 0, maxY);
				}
				break;
			}
			}
		}

		virtual void scrollMove(MouseEventArgs* args, Viewport* viewport) {
			auto& prop = *viewport->getProperties<Viewport::Properties>();
			int maxX = prop.contentRect.width - prop.viewportRect.width;
			int maxY = prop.contentRect.height - prop.viewportRect.height;
			switch (args->wheel.Direction) {
			case MouseWheelArgs::Horizontal: {
				if (prop.moveMode == ScrollMode::MoveContent) {
					// 移动内容：根据鼠标滚轮的垂直滚动更新内容位置
					int newX = prop.contentRect.x - static_cast<int>(args->wheel.Delta);
					prop.contentRect.x = std::clamp(newX, 0, maxX);
					viewport->horizontalScrollbar.setSliderProgeress((double)prop.contentRect.x / (double)maxX);
				}
				else { // Viewport
					// 移动视口：根据鼠标滚轮的垂直滚动更新视口位置
					int newX = prop.viewportRect.x - static_cast<int>(args->wheel.Delta);
					prop.viewportRect.x = std::clamp(newX, 0, maxX);
					viewport->horizontalScrollbar.setSliderProgeress((double)prop.viewportRect.x / (double)maxX);
				}
				break;
			}
			case MouseWheelArgs::Vertical: {
				if (prop.moveMode == ScrollMode::MoveContent) {
					// 移动内容：根据鼠标滚轮的垂直滚动更新内容位置
					int newY = prop.contentRect.y - static_cast<int>(args->wheel.Delta);
					prop.contentRect.y = std::clamp(newY, 0, maxY);
					viewport->verticalScrollbar.setSliderProgeress((double)prop.contentRect.y / (double)maxY);
				}
				else { // Viewport
					// 移动视口：根据鼠标滚轮的垂直滚动更新视口位置
					int newY = prop.viewportRect.y - static_cast<int>(args->wheel.Delta);
					prop.viewportRect.y = std::clamp(newY, 0, maxY);
					viewport->verticalScrollbar.setSliderProgeress((double)prop.viewportRect.y / (double)maxY);
				}
				break;
			}
			}
		}

		void SetFlagScrollingInProgress(bool flag) {
			scrollingInProgress = flag;
		}
	};
protected:
	ScreenA internalScreen;
	CanvasA canvas;
	Logger* logger;
	Displayer* display;

	BufferDispatcherA dispatcher;
	Scrollbar horizontalScrollbar;
	Scrollbar verticalScrollbar;

public:
	Viewport()
		: internalScreen(nullptr), dispatcher(nullptr) {
	};
	[[deprecated]] void Init() override {};
	virtual void Init(ScreenA* screen, Logger* logger, Displayer* display) {
		this->internalScreen.sendSignal(ScreenA::Signal::SetBlender, &screen->getBlender());
		this->canvas.attach(&this->internalScreen);
		this->logger = logger;
		this->display = display;
		dispatcher.attach(screen);
		SetStyle(new Style());
		InitScrollbar();
		SetDrawer([this](ScreenA* screen, CanvasA* canvas, KeyFrameGroupLoader loader, void* args) {
			style->DrawPrimitive(*screen, *canvas, this);
		});
		addEvent(Events::_MouseEvent_, [this](void* _args, ActionableWidget* widget) {
			style->MouseEvent(static_cast<MouseEventArgs*>(_args), widget);
		});
	}
protected:
	virtual void InitScrollbar() {
		auto& prop = *getProperties<Viewport::Properties>();
		horizontalScrollbar.Init();
		horizontalScrollbar.SetProperties(new Scrollbar::Properties());
		horizontalScrollbar.addEvent(Slider::onSliderValueChanged, [this](void* args, ActionableWidget* widget) {
			Scrollbar* scrollbar = widget->as<Scrollbar>();
			auto& style = *getStyle<Style>();
			style.scrollByScrollbar(scrollbar, Direction::Horizontal, this);
			style.SetFlagScrollingInProgress(true);
		});
		horizontalScrollbar.addEvent(Scrollbar::Events::onScrollDragCancel, [this](void* args, ActionableWidget* widget) {
			auto& style = *getStyle<Style>();
			style.SetFlagScrollingInProgress(false);
		});

		verticalScrollbar.Init();
		verticalScrollbar.SetProperties(new Scrollbar::Properties());
		verticalScrollbar.addEvent(Slider::onSliderValueChanged, [this](void* args, ActionableWidget* widget) {
			Scrollbar* scrollbar = widget->as<Scrollbar>();
			auto& style = *getStyle<Style>();
			style.scrollByScrollbar(scrollbar, Direction::Vertical, this);
			style.SetFlagScrollingInProgress(true);
		});
		verticalScrollbar.addEvent(Scrollbar::Events::onScrollDragCancel, [this](void* args, ActionableWidget* widget) {
			auto& style = *getStyle<Style>();
			style.SetFlagScrollingInProgress(false);
			});

	}
public:
	virtual void Render(ScreenA& screen, CanvasA& canvas) override {
		auto& prop = *getProperties<Properties>();
		auto& hscrollProp = *horizontalScrollbar.getProperties<Scrollbar::Properties>();
		auto& vscrollProp = *verticalScrollbar.getProperties<Scrollbar::Properties>();
		this->internalScreen.resize(prop.contentRect.width, prop.contentRect.height);
		animator.DrawUnified(&this->internalScreen, &this->canvas, static_cast<void*>(this));
		//animator.DrawUnified(&screen, &canvas, static_cast<void*>(this));
		logger->LogOverlayRender(screen, canvas);
		dispatcher.SendBufferData(this->internalScreen.getBuffer(), prop.viewportRect.x, prop.viewportRect.y,
			prop.contentRect.x, prop.contentRect.x + prop.contentRect.width + vscrollProp.Width, prop.contentRect.y, prop.contentRect.y + prop.contentRect.height + hscrollProp.Height,
			prop.charOnly, prop.blendMode, prop.multiplyAlpha
		);
	}

	virtual void SetScrollbarSize(int HscrollHeight, int VscrollWidth) {
		auto& hscrollProp = *horizontalScrollbar.getProperties<Scrollbar::Properties>();
		auto& vscrollProp = *verticalScrollbar.getProperties<Scrollbar::Properties>();
		hscrollProp.Height = HscrollHeight;
		vscrollProp.Width = VscrollWidth;
	}

	virtual void UpdateScrollBar() {
		auto& prop = *getProperties<Viewport::Properties>();
		auto& hscrollProp = *horizontalScrollbar.getProperties<Scrollbar::Properties>();
		auto& vscrollProp = *verticalScrollbar.getProperties<Scrollbar::Properties>();
		if (prop.moveMode == ScrollMode::MoveContent) {
			hscrollProp.X = prop.viewportRect.x;
			hscrollProp.Y = prop.viewportRect.y + prop.viewportRect.height;
			hscrollProp.Width = prop.viewportRect.width;

			vscrollProp.X = prop.viewportRect.x + prop.viewportRect.width;
			vscrollProp.Y = prop.viewportRect.y;
			vscrollProp.Height = prop.viewportRect.height;
		}
		else { // MoveMode::Viewport
			hscrollProp.X = prop.contentRect.x;
			hscrollProp.Y = prop.contentRect.y + prop.contentRect.height;
			hscrollProp.Width = prop.contentRect.width;

			vscrollProp.X = prop.contentRect.x + prop.contentRect.width;
			vscrollProp.Y = prop.contentRect.y;
			vscrollProp.Height = prop.contentRect.height;
		}
		hscrollProp.viewportLength = prop.viewportRect.width;
		hscrollProp.contentLength = prop.contentRect.width;
		hscrollProp.direction = Slider::Direction::Horizontal;

		vscrollProp.viewportLength = prop.viewportRect.height;
		vscrollProp.contentLength = prop.contentRect.height;
		vscrollProp.direction = Slider::Direction::Vertical;
	}

	virtual void setRect(int x, int y, UINT width, UINT height, int contentX, int contentY, UINT contentWidth, UINT contentHeight) {
		auto& prop = *getProperties<Properties>();
		prop.viewportRect = { x,y,width,height };
		prop.contentRect = { contentX ,contentY ,contentWidth,contentHeight };
	}

	void setMoveMode(ScrollMode mode) {
		getProperties<Properties>()->moveMode = mode;
	}

	void setBlendMode(Pixel::BlendMode mode) {
		getProperties<Properties>()->blendMode = mode;
	}

	void setCharOnly(bool flag) {
		getProperties<Properties>()->charOnly = flag;
	}

	void setMultiplyAlpha(float alpha) {
		getProperties<Properties>()->multiplyAlpha = alpha;
	}

	void setContentPosition(int x, int y) {
		auto& prop = *getProperties<Properties>();
		prop.contentRect.x = x;
		prop.contentRect.y = y;
	}

	void setContentSize(int width, int height) {
		auto& prop = *getProperties<Properties>();
		prop.contentRect.width = width;
		prop.contentRect.height = height;
	}

	void setViewportPosition(int x, int y) {
		auto& prop = *getProperties<Properties>();
		prop.viewportRect.x = x;
		prop.viewportRect.y = y;
	}

	void setViewportSize(int width, int height) {
		auto& prop = *getProperties<Properties>();
		prop.viewportRect.width = width;
		prop.viewportRect.height = height;
	}

	// 获取视口内显示的内容的偏移位置
	Rect getViewportRect() { return getProperties<Properties>()->viewportRect; }

	Rect getContentRect() { return getProperties<Properties>()->contentRect; }

	ScrollMode getMoveMode() { return getProperties<Properties>()->moveMode; }

	Pixel::BlendMode getBlendMode() { return getProperties<Properties>()->blendMode; }

	bool getCharOnly() { return getProperties<Properties>()->charOnly; }

	float getMultiplyAlpha() { return getProperties<Properties>()->multiplyAlpha; };
};