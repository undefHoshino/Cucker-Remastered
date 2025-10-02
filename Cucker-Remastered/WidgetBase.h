#pragma once
#include <functional>
#include <unordered_map>
#include "Layer.h"
#include "Signal.h"
#include "InputCondition.h"
#include "Container.h"
#include "PosCalculator.h"
#include "Animator.h"
#include "EventDispatcher.h"

class Widget : public Layer {
public:
	class WidgetProperties {
	public:
		virtual ~WidgetProperties() = default;  // 确保子类能正确析构
	};

	class RectProperties : public WidgetProperties {
	public:
		int X;
		int Y;
		UINT Width;
		UINT Height;

		RectProperties() : X(0), Y(0), Width(10), Height(3) {}

		RectProperties(int x, int y, UINT width, UINT height)
			: X(x), Y(y), Width(width), Height(height) {}
	};

	class WidgetData {
	public:
		virtual ~WidgetData() = default;
	};

	class WidgetStyle {
	public:
		virtual void DrawPrimitive(ScreenA& screen, CanvasA& canvas, Widget* widget) {}
		virtual void MouseEvent(MouseEventArgs* args, Widget* widget) {};
		virtual void KeyEvent(KeyEventArgs* args, Widget* widget) {};
		virtual void WinFocusEvent(WinFocusEventArgs* args, Widget* widget) {};
		virtual void WinMenuEvent(WinMenuEventArgs* args, Widget* widget) {};
		virtual void WinBufferSizeEvent(WinBufferEventArgs* args, Widget* widget) {};
		virtual void UpdateEvent(Widget* widget) {}
	};
protected:
	WidgetProperties* properties = nullptr;
	WidgetData* data = nullptr;
	WidgetStyle* style = nullptr;

	SignalManager signalManager;
	MapContainer container;
	Animator animator;
public:
	virtual void Init();
	virtual void SendSignal(int signal) {};
	virtual void HandleSignal() {};
	virtual void Render(ScreenA& screen, CanvasA& canvas) override;
	void SetProperties(WidgetProperties* prop);
	Animator& getAnimator();
	MapContainer& getContainer();

	template<class T>
	T* getData() {
		return static_cast<T*>(data);
	}
	template<class T>
	T* getProperties() {
		return static_cast<T*>(properties);
	}
	template<class T>
	T* getStyle() {
		return static_cast<T*>(style);
	}
	template<class T>
	T* as() {
		return static_cast<T*>(this);
	}

	~Widget();
protected:
	void SetDrawer(TimeAxis::Drawer::DrawFunction func);
	void SetData(WidgetData* data);
	void SetStyle(WidgetStyle* style);
};

class ActionableWidget : public Widget {
public:
	enum Events {
		_MouseEvent_			= -1,  // MouseEventArgs
		_KeyEvent_				= -2,  // KeyEventArgs
		_WinFocusEvent_			= -3,  // WinFocusEventArgs
		_WinMenuEvent_			= -4,  // WinMenuEventArgs
		_WinBufferSizeEvent_	= -5,  // WinBufferEventArgs
		_UpdateEvent_			= -6,  // Update
		_AcquireCaptureEvent_		= -7,  // gain Capture
		_LostCaptureEvent_		= -8,  // lost Capture
		_AcquireFocusEvent_		= -9,  // gain Focus
		_LostFocusEvent_		= -10, // lost Focus
	};
protected:
	using _EventFunction = std::function<void(void*, ActionableWidget*)>;
	using _DrawFunction = Timeline::Drawer;

	EventDispatcher<int, _EventFunction, void*, ActionableWidget*> eventDispatcher;
public:
	virtual void Mouse(MouseEventArgs args) override;
	virtual void Key(KeyEventArgs args) override;
	virtual void WinFocus(WinFocusEventArgs args) override;
	virtual void WinMenu(WinMenuEventArgs args) override;
	virtual void WinBufferSize(WinBufferEventArgs args) override;
	virtual void Render(ScreenA& screen, CanvasA& canvas) override;
	virtual void Update() override;

	virtual void SetWidgetRectPosition(int x, int y);
	virtual void SetWidgetRectSize(UINT width, UINT height);

	void addEvent(int event, _EventFunction func);
	void removeEvent(int event);
	void refreshEvent(int event);
	void callEvent(int event, void* args, bool quiet = false);
protected:
	bool existEvent(int event);
};

class InputStateManager {
public:
	virtual bool MouseInput(MouseEventArgs* args, ActionableWidget* widget, bool isInArea) { return true; };
	virtual void MouseBackground(MouseEventArgs* args) {};
	virtual ~InputStateManager() = default;
};


class MouseCapture: public InputStateManager {
private:
	MouseCapture() = default;
public:
	bool capturing = false;
	bool backgroundCaptured = false;
	ActionableWidget* capturedWidget = nullptr;
public:
	MouseCapture(const MouseCapture&) = delete;
	MouseCapture& operator=(const MouseCapture&) = delete;

	static MouseCapture& getInstance();
	bool MouseInput(MouseEventArgs* args, ActionableWidget* widget, bool isInArea) override;
	void MouseBackground(MouseEventArgs* args) override;
	void capture(ActionableWidget* widget);
	void release();
	bool isCaptured() const;
	ActionableWidget* getCapturedWidget() const;
	bool shouldDeliverTo(ActionableWidget* widget) const;
};


class WidgetFocus : public InputStateManager {
private:
	WidgetFocus() = default;
public:
	bool isHitWidget = false;
	bool backgroundFocus = false;
	ActionableWidget* focusedWidget = nullptr;
public:
	WidgetFocus(const WidgetFocus&) = delete;
	WidgetFocus& operator=(const WidgetFocus&) = delete;

	static WidgetFocus& getInstance();
	bool MouseInput(MouseEventArgs* args, ActionableWidget* widget, bool isInArea) override;
	void MouseBackground(MouseEventArgs* args) override;
	void setFocus(ActionableWidget* widget);
	void clearFocus();
	bool hasFocusedWidget();
	ActionableWidget* getFocusedWidget() const;
};


class UnifiedStateManager: public InputStateManager {
private:
	MouseCapture* mouseCaptureInst = nullptr;
	WidgetFocus* widgetFocusInst = nullptr;

	UnifiedStateManager();
public:
	UnifiedStateManager(const UnifiedStateManager&) = delete;
	UnifiedStateManager& operator=(const UnifiedStateManager&) = delete;

	static UnifiedStateManager& getInstance();
	bool MouseInput(MouseEventArgs* args, ActionableWidget* widget, bool isInArea) override;
	void MouseBackground(MouseEventArgs* args) override;
	bool isCaptured();
	bool isFocused();
};