#pragma once
#include "WidgetBase.h"

class Button : public ActionableWidget {
public:
	struct Properties : public RectProperties {
		std::string Caption;
		Pixel rectBoxColor = { {255,255,255,100},{} };

		Properties() = default;
		Properties(int x, int y, UINT width, UINT height, std::string caption, Pixel rectBoxColor = { {255,255,255,100},{} }) :
			RectProperties(x, y, width, height), Caption(caption), rectBoxColor(rectBoxColor) {};
	};
	class Style : public WidgetStyle {
	protected:
		int ButtonIndex = 0;
	public:
		virtual void DrawPrimitive(ScreenA& screen, CanvasA& canvas, Widget* widget) override;
		virtual void MouseEvent(MouseEventArgs* args, Widget* widget) override;
		void setIndex(int buttonIndex);
		int getIndex();
	};
public:
	Button() {}
	virtual void Init() override;
};

class NoobButton : public Button {
public:
	class Style : public Button::Style {
	protected:
		MouseEventHandler::ButtonPressHandler bsHandler;
		bool isButtonSelected = false;
		bool isSelectionCanceled = false;
	public:
		virtual void MouseEvent(MouseEventArgs* args, Widget* widget) override;
		virtual void KeyEvent(KeyEventArgs* args, Widget* widget) override;
		void selectButton();
		void deselectButton();
	};
public:
	struct MouseEventArgsEx {
		MouseEventHandler::ButtonPressHandler::Result* pressResult = nullptr;
		MouseEventArgs args;
		bool inArea = false;
	};

	// Based on _MouseEvent_, _KeyEvent_
	enum Events {
		_MouseEventEx_ = 0,				// MouseEventArgsEx
		onMouseHeld = 1,				// MouseEventArgs
		onMouseReleased = 2,			// MouseEventArgs
		onMousePressed = 3,				// MouseEventArgs
		onMouseDoubleClick = 4,			// MouseEventArgs
		onMouseInArea = 5,				// MouseEventArgs
		onButtonSelected = 6,			// KeyEventArgs
		onButtonSelectionCanceled = 7,  // KeyEventArgs | Executes once
	};

	virtual void Init() override;
	virtual void SetButtonCaption(std::string caption);
};

class RadioButton : public NoobButton {
public:
	struct Properties : public NoobButton::Properties {
	public:
		std::string toggleEnableString = "[Enable]";
		std::string toggleDisableString = "[Disable]";
		Properties() = default;
		Properties(int x, int y, std::string enableString = "[Enable]", std::string disableString = "[Disable]", Pixel rectBoxColor = { {255,255,255,100},{} }) :
			NoobButton::Properties(x, y, 10, 1, "", rectBoxColor), toggleEnableString(enableString), toggleDisableString(disableString) {};
	};

	class Style : public NoobButton::Style {
	private:
		bool stateToggle = false;
	public:
		virtual void DrawPrimitive(ScreenA& screen, CanvasA& canvas, Widget* widget) override;
		void setStateToggle(bool flag);
		bool getStateToggle();
	};
	virtual void Init() override;
	virtual void setStateToggle(bool flag);
	virtual bool getStateToggle();
};

// Unfinished
class ButtonSelectHandler {
private:
	std::vector<std::map<int, NoobButton::Style*>> layers;
	size_t thisLayer = 0;
public:
	void pushLayer() {
		layers.push_back({});
	}
	void popLayer() {
		layers.pop_back();
	}
	void selectLayer(size_t layer) {
		checkLayer(layer);
		thisLayer = layer;
	}
	void removeLayer(size_t layer) {
		checkLayer(layer);
		layers.erase(layers.begin() + layer);
	}
	void addButton(int id, NoobButton::Style* style) {
		layers[thisLayer].insert(std::make_pair(id, style));
	}
	void removeButton(int id) {
		layers[thisLayer].erase(id);
	}
	void checkLayer(size_t layer) {
		if (layer >= layers.size()) throw std::out_of_range("Layer out of range.");
	}
	void KeyEvent(KeyEventArgs args) {

	}
};