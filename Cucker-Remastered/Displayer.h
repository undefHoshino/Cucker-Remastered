#pragma once
#include "Interface.h"
#include "Engine.h"

class Displayer : public ConsoleEngine::Component {
protected:
	ScreenA screen;
	ScreenBlenderA blender;
	CanvasA canvas;

	std::unordered_map<std::string, Interface*> manager;
	Interface* current = nullptr;
public:
	LogOverlay& logOverlay = LogOverlay::GetInstance();

	Displayer() :screen(&blender), canvas(&screen) {};
	void Init(ConsoleEngine& source) override {
		ConsoleEngine::Component::Init(source);
	}
	void Insert(const std::string& id, Interface* iface) {
		iface->Creation(&screen, &canvas, &logOverlay, this);
		manager.insert(std::make_pair(id, iface));
	}
	void Navigate(const std::string& id) {
		if (!exist(id)) return;

		unload();
		load(id);
	}
	void Free() {
		for (auto& ptr : manager) {
			delete ptr.second;
		}
		manager.clear();
	}
	Interface* Get(const std::string& id) {
		if (!exist(id)) throw ReportException("Interface (", id, ") doesn't exist");
		return manager[id];
	}
	~Displayer() {
		Free();
	}
protected:
	void unload() {
		if (current) current->Unload();
	}
	bool load(const std::string& id) {
		if (!exist(id)) return false;
		manager[id]->Load();
		current = manager[id];

		return true;
	}
	bool exist(std::string id) {
		return manager.count(id) > 0;
	}
public:
	void Render() {
		current->onCreateScreen();
		current->Render();
		current->onWriteConsole();
	}
	void BackgroundLogic() {
		current->BackgroundLogic();
	}
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
			throw ReportException("Unsupported EventArgs in SendInput");
		}
	}
};