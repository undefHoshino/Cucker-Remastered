#include "Interface.h"

void Interface::Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Display* display) {
    this->screen = screen;
    this->canvas = canvas;
    this->logger = logger;
    this->display = display;
};

void Interface::onCreateScreen() {
    CONSOLE_SCREEN_BUFFER_INFO csbi = GetConsoleScreenBuffer();
    screen->resize(GetConsoleWindowWidth(csbi), GetConsoleWindowHeight(csbi));
}

void Interface::onWriteConsole() {
    logger->Render(*screen, *canvas);
    screen->execute();
    screen->write();
}

void Display::Insert(int id, Interface* iface) {
    iface->Creation(&modules.screen, &modules.canvas, modules.logger, this);
    manager.insert(std::make_pair(id, iface));
}

void Display::Navigate(int id) {
    if (exist(id)) {
        unload(ifacePointer);

        if (load(id)) {
            this->ifacePointer = id;
        }
    }
}

void Display::BackgroundLogic() {
    manager[ifacePointer]->BackgroundLogic();
}

void Display::Render() {
    auto& iface = manager[ifacePointer];

    iface->onCreateScreen();
    iface->Render();
    iface->onWriteConsole();
}

Interface* Display::GetInterface(int id) {
    if (exist(id)) {
        return manager[id];
    }
    throw std::runtime_error("Interface (" + std::to_string(id) + ") doesn't exist");
}

bool Display::exist(int id) {
    return manager.count(id);
}

bool Display::load(int id) {
    if (exist(id)) {
        manager[id]->Load();
        return true;
    }
    return false;
}

bool Display::unload(int id) {
    if (exist(id)) {
        manager[id]->Unload();
        return true;
    }
    return false;
}

Display::~Display() {
    for (auto& ptr : manager) {
        delete ptr.second;
    }
    manager.clear();
}