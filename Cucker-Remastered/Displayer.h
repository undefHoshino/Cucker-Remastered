#pragma once
#include "Interface.h"
#include "Engine.h"

class Displayer : public EngineComponent {
protected:
	ScreenA screen;
	ScreenBlenderA blender;
	CanvasA canvas;

	std::unordered_map<std::string, Interface*> manager;
	Interface* current = nullptr;
public:
	LogOverlay& logOverlay = LogOverlay::GetInstance();

	Displayer() :screen(&blender), canvas(&screen) {};
	void Insert(const std::string& id, Interface* iface);
	void Navigate(const std::string& id);
	void Free();
	Interface* Get(const std::string& id);
	~Displayer();
protected:
	void unload();
	bool load(const std::string& id);
	bool exist(std::string id);
public:
	void Render();
	void BackgroundLogic();
	template<class EventArgs, class T>
	void SendInputArgs(T args) {
		if constexpr (std::is_same_v<EventArgs, MouseEventArgs>) {
			current->Mouse(args);
		}
		else if constexpr (std::is_same_v<EventArgs, KeyEventArgs>) {
			current->Key(args);
		}
		else if constexpr (std::is_same_v<EventArgs, FocusEventArgs>) {
			current->Focus(args);
		}
		else if constexpr (std::is_same_v<EventArgs, MenuEventArgs>) {
			current->Menu(args);
		}
		else if constexpr (std::is_same_v<EventArgs, BufferEventArgs>) {
			current->BufferSize(args);
		}
		else {
			throw ReportException("Unsupported EventArgs in SendInputArgs");
		}
	}
};