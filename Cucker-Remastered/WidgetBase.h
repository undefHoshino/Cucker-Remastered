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
		virtual void FocusEvent(FocusEventArgs* args, Widget* widget) {};
		virtual void MenuEvent(MenuEventArgs* args, Widget* widget) {};
		virtual void BufferSizeEvent(BufferEventArgs* args, Widget* widget) {};
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
		_MouseEvent_ = -1,  // MouseEventArgs
		_KeyEvent_ = -2,  // KeyEventArgs
		_FocusEvent_ = -3,  // FocusEventArgs
		_MenuEvent_ = -4,  // MenuEventArgs
		_BufferSizeEvent_ = -5,  // BufferEventArgs
		_UpdateEvent_ = -6   // 
	};
protected:
	using _EventFunction = std::function<void(void*, ActionableWidget*)>;
	using _DrawFunction = Timeline::Drawer;

	EventDispatcher<int, _EventFunction, void*, ActionableWidget*> eventDispatcher;
public:
	virtual void Mouse(MouseEventArgs args) override;
	virtual void Key(KeyEventArgs args) override;
	virtual void Focus(FocusEventArgs args) override;
	virtual void Menu(MenuEventArgs args) override;
	virtual void BufferSize(BufferEventArgs args) override;
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