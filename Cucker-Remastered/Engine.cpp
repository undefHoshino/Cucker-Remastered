#include "Engine.h"

void ConsoleEngine::Initialization() {
	Register();
};

void ConsoleEngine::Run() {
	for (auto& com : component) {
		com.second->Start();
	}
}

bool ConsoleEngine::ExistComponent(const std::type_info& type) {
	return component.find(&type) != component.end();
}

void ConsoleEngine::Free() {
	for (auto& ptr : component) {
		delete ptr.second;
		ptr.second = nullptr;
	}
	component.clear();
}

ConsoleEngine::~ConsoleEngine() {
	Free();
}