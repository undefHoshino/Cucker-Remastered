#pragma once
#include <functional>
#include <map>
#include <any>
#include <variant>
#include <unordered_map>
#include "Screen.h"
#include "Canvas.h"
#include "HpetClock.h"

class KeyFrame {
public:
    using Easing = std::function<double(double)>;
    struct Point {
        Point() = default;
        Point(uint64_t startTime, uint64_t duration, double value, Easing easing = [](double t)->double { return t; }) :
            startTime(startTime), duration(duration), value(value), easing(easing) {}

        uint64_t startTime = 0;
        uint64_t duration = 0;
        double value = 0;
        Easing easing = [](double t)->double { return t; };
    };
    struct Data {
        int64_t offset = 0;
        std::vector<Point> frames;

        uint64_t presentTime = 0;
    };
private:
    Data data;
public:
    KeyFrame() = default;
    KeyFrame(int64_t offset);
    KeyFrame& setOffset(int64_t offset);
    KeyFrame& push(uint64_t duration, double value, Easing easing = [](double t)->double { return t; });
    void clear();
    void shrink();
    std::vector<Point> getFrames() const;
    Data getData() const;
    int64_t getOffset() const;
    uint64_t calcTotalDuration(bool offset = true, bool ignoreLast = true) const;
};

using KeyFrameLists = std::unordered_map<std::string, KeyFrame>;
using KeyFramePoints = std::unordered_map<std::string, KeyFrame::Point>;

class KeyFramePointGenerator {
private:
    using Easing = std::function<double(double)>;
    KeyFramePoints data;
public:
    void push(std::string name, uint64_t duration, double value, Easing easing = [](double t)->double { return t; }) {
        data.insert(std::make_pair(name, KeyFrame::Point{ 0,duration,value,easing }));
    }
    void clear() {
        data.clear();
    }
    KeyFramePoints getData() const {
        return data;
    }
};

class FrameInterpolator {
protected:
    struct FrameContext {
        KeyFrame::Point before;
        KeyFrame::Point after;
        uint64_t accumulatedTime = 0;
        uint64_t inDurationTime = 0;
        bool isEnded = false;
        bool started = true;
    };
    static FrameContext findFrames(KeyFrame& data, int64_t realElapsedTime);
    static KeyFrame::Point calculate(KeyFrame::Point before, KeyFrame::Point after,
        uint64_t inDurationTime, uint64_t duration, KeyFrame::Easing easing = [](double t)->double { return t; });
public:
    static KeyFramePoints Interpolate(KeyFramePoints start, KeyFramePoints end, int64_t elapsedTime, uint64_t duration, int64_t offset, KeyFrame::Easing easing);
    static KeyFramePoints Interpolate(KeyFrameLists& data, int64_t elapsedTime);
};

template<class Loader>
struct FrameDrawer {
    using DrawFunction = std::function<void(ScreenA*, CanvasA*, Loader, void*)>;
    std::shared_ptr<DrawFunction> func;
    void Make(DrawFunction frameDrawer) {
        func = std::make_shared<DrawFunction>(frameDrawer);
    }
    void Redirect(DrawFunction* ptr) {
        func = std::shared_ptr<DrawFunction>(ptr);
    }
    void Draw(ScreenA* screen, CanvasA* canvas, Loader& loader, void* args) {
        (*func)(screen, canvas, loader, args);
    }
    DrawFunction* getPtr() {
        return func.get();
    }
};
