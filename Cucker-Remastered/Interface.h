#pragma once
#include "Screen.h"
#include "Canvas.h"
#include "LogOverlay.h"
#include "FpsOverlay.h"
#include "InputHandler.h"
#include "ConsoleFunction.h"

class Display;

class Interface {
protected:
    ScreenA* screen = nullptr;
    CanvasA* canvas = nullptr;
    LogOverlay* logger = nullptr;
    Display* display = nullptr;
public:
    virtual void Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Display* display);
    virtual void Load() {};
    virtual void Unload() {};
    virtual void Render() {};
    virtual void Mouse(MouseEventArgs args) {}
    virtual void Key(KeyEventArgs args) {}
    virtual void Focus(FocusEventArgs args) {}
    virtual void Menu(MenuEventArgs args) {}
    virtual void BufferSize(BufferEventArgs args) {}
    virtual void BackgroundLogic() {};
    virtual void onCreateScreen();
    virtual void onWriteConsole();
};

template<class _Fty>
class InterfaceManager {
public:
    enum InvokeMethod {
        Deque,
        Pointer
    };
private:
    std::unordered_map<_Fty, Interface*> interfaces;
    std::vector<_Fty> ifdeque;
    Interface* activeInterface;
    int invokeMethod = InvokeMethod::Deque;
public:
#define _InterfaceInvoke(func)  \
if (invokeMethod == InvokeMethod::Deque) { \
    for (auto& _fty : ifdeque) { interfaces[_fty]->func; } \
} else { \
    activeInterface->func; \
}
    void Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Display* display) {
        _InterfaceInvoke(Creation(screen, canvas, logger, display));
    }
    void Load() {
        _InterfaceInvoke(Load());
    };
    void Unload() {
        _InterfaceInvoke(Unload());
    };
    void Render() {
        _InterfaceInvoke(Render());
    }
    void Mouse(MouseEventArgs args) {
        _InterfaceInvoke(Mouse(args));
    }
    void Key(KeyEventArgs args) {
        _InterfaceInvoke(Key(args));
    }
    void Focus(FocusEventArgs args) {
        _InterfaceInvoke(Focus(args));
    }
    void Menu(MenuEventArgs args) {
        _InterfaceInvoke(Menu(args));
    }
    void BufferSize(BufferEventArgs args) {
        _InterfaceInvoke(BufferSize(args));
    }
    void BackgroundLogic() {
        _InterfaceInvoke(BackgroundLogic());
    }
    void onCreateScreen() {
        _InterfaceInvoke(onCreateScreen());
    }
    void onWriteConsole() {
        _InterfaceInvoke(onWriteConsole());
    }
#undef _InterfaceInvoke
    void addInterface(_Fty _fty, Interface* iface) {
        interfaces.insert(std::make_pair(_fty, iface));
    }
    void removeInterface(_Fty _fty) {
        if (hasInterface(_fty))
            interfaces.erase(_fty);
    }
    void addDeque(const _Fty& _fty) {
        if (!hasInterface(_fty)) throwNotExistException(_fty);
        interfaces[_fty]->Load();
        ifdeque.push_back(_fty);
    }
    void removeDeque(const _Fty& _fty) {
        auto it = std::find(ifdeque.begin(), ifdeque.end(), _fty);
        if (it != ifdeque.end()) {
            ifdeque.erase(it);
            interfaces[_fty]->Unload();
        }
    }
    bool hasInterface(_Fty _fty) {
        return interfaces.count(_fty) > 0;
    }
    void UsingInvoke(int invokeMethod) {
        this->invokeMethod = invokeMethod;
    }
    void activateInterface(const _Fty& _fty) {
        if (!hasInterface(_fty)) throwNotExistException(_fty);
        if (this->activeInterface) {
            this->activeInterface->Unload();
        }
        this->activeInterface = interfaces[_fty];
        this->activeInterface->Load();
    }
    void deactivateInterface() {
        if (this->activeInterface) {
            this->activeInterface->Unload();
        }
        this->activeInterface = nullptr;
    }
    void throwNotExistException(const _Fty& _fty) {
        std::ostringstream oss;
        oss << _fty;
        throw std::runtime_error("Interface (named " + oss.str() + ") doesn't exist");
    }
    Interface* getScreen(const _Fty& _fty) {
        if (!hasInterface(_fty)) throwNotExistException(_fty);
        return interfaces[_fty];
    }
    Interface* getActivatedInterface() {
        return this->activeInterface;
    }
};

class Display {
public:
    struct Module {
        ScreenA screen;
        ScreenBlenderA blender;
        CanvasA canvas;
        LogOverlay* logger = nullptr;

        Module(LogOverlay* logOverlay) :screen(&blender), canvas(&screen), logger(logOverlay) {};
    };
private:
    std::unordered_map<int, Interface*> manager;
    Module modules;

    int ifacePointer = 0;
public:
    Display(LogOverlay* logger) :modules(logger) {};
    void Insert(int id, Interface* iface);
    void Navigate(int id);
    void Render();
    void BackgroundLogic();
    bool exist(int id);
    ~Display();

    Interface* GetInterface(int id);
private:
    bool load(int id);
    bool unload(int id);
public:
    template<class EventArgs, class T>
    void SendInputArgs(T args) {
        auto& iface = manager[ifacePointer];

        if constexpr (std::is_same_v<EventArgs, MouseEventArgs>) {
            iface->Mouse(args);
        }
        else if constexpr (std::is_same_v<EventArgs, KeyEventArgs>) {
            iface->Key(args);
        }
        else if constexpr (std::is_same_v<EventArgs, FocusEventArgs>) {
            iface->Focus(args);
        }
        else if constexpr (std::is_same_v<EventArgs, MenuEventArgs>) {
            iface->Menu(args);
        }
        else if constexpr (std::is_same_v<EventArgs, BufferEventArgs>) {
            iface->BufferSize(args);
        }
        else {
            throw std::runtime_error("Unsupported EventArgs in SendInput");
        }
    }

};

