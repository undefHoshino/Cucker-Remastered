#include "Displayer.h"

void Displayer::Insert(const std::string& id, Interface* iface) {
	iface->Init(parent, 0);
	iface->Creation(&screen, &canvas, &logOverlay, this);
	manager.insert(std::make_pair(id, iface));
}

void Displayer::Navigate(const std::string& id) {
	if (!exist(id)) return;

	unload();
	load(id);
}

void Displayer::Free() {
	for (auto& ptr : manager) {
		delete ptr.second;
	}
	manager.clear();
}

Interface* Displayer::Get(const std::string& id) {
	if (!exist(id)) throw ReportException("Interface (", id, ") doesn't exist");
	return manager[id];
}

void Displayer::unload() {
	if (current) current->Unload();
}

bool Displayer::load(const std::string& id) {
	if (!exist(id)) return false;
	manager[id]->Load();
	current = manager[id];

	return true;
}

bool Displayer::exist(std::string id) {
	return manager.count(id) > 0;
}

void Displayer::Render() {
	current->onCreateScreen();
	current->Render();
	current->onWriteConsole();
}

void Displayer::BackgroundLogic() {
	current->BackgroundLogic();
}

Displayer::~Displayer() {
	Free();
}