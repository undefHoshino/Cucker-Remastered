#pragma once
#include <unordered_map>
#include <string>
#include <functional>
#include <vector>

template<class _Fty>
class EventVector {
private:
    using Function = std::function<bool(std::pair<_Fty, void*>)>;
    using Container = std::vector<std::pair<_Fty, void*>>;
    Container container;
public:
    void push(int sign, void* args) {
        container.push_back(std::make_pair(sign, args));
    }
    Container get() {
        return container;
    }
    void list(Function func) {
        for (auto& ptr : container) {
            if (!func(ptr)) break;
        }
    }
    bool empty() {
        return container.empty();
    }
    void clear() {
        container.clear();
    }
    void shrink() {
        container.shrink_to_fit();
    }
};

template<class _Fty, class _Function, class ..._Args>
class EventDispatcher {
private:
    std::unordered_map<_Fty, std::vector<_Function>> eventHandlers;

public:
    void addEvent(const _Fty& event, const _Function& handler) {
        eventHandlers[event].push_back(handler);
    }

    void dispatchEvent(const _Fty& event, _Args&&... args) {
        if (existEvent(event)) {
            for (auto& handler : eventHandlers[event]) {
                handler(std::forward<_Args>(args)...);
            }
        }
    }

    void removeEvent(const _Fty& event) {
        eventHandlers.erase(event);
    }

    bool existEvent(const _Fty& event) {
        return eventHandlers.count(event);
    }

    void refreshEvent(const _Fty& event) {
        if (existEvent(event)) {
            eventHandlers[event].clear();
        }
    }
};