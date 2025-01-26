#include "KeyFrame.h"

KeyFrame::KeyFrame(int64_t offset) {
    setOffset(offset);
}
KeyFrame& KeyFrame::setOffset(int64_t offset) {
    data.offset = offset;
    return *this;
}
KeyFrame& KeyFrame::push(uint64_t duration, double value, Easing easing) {
    data.frames.push_back(Point(data.presentTime,duration,value,easing));
    data.presentTime += duration;
    return *this;
}
void KeyFrame::clear() {
    data.frames.clear();
    data.presentTime = 0;
}
void KeyFrame::shrink() {
    data.frames.shrink_to_fit();
}
std::vector<KeyFrame::Point> KeyFrame::getFrames() const {
    return data.frames;
}
KeyFrame::Data KeyFrame::getData() const {
    return data;
}
int64_t KeyFrame::getOffset() const {
    return data.offset;
}
uint64_t KeyFrame::calcTotalDuration(bool offset, bool ignoreLast) const {
    if (data.frames.empty())
        return offset ? data.offset : 0;

    // 获取最后一个帧和其时长
    const auto& lastFrame = *data.frames.rbegin();
    uint64_t result = lastFrame.startTime + lastFrame.duration;

    if (offset)
        result += data.offset;
    if (ignoreLast)
        result -= lastFrame.duration;
    return result;
}

FrameInterpolator::FrameContext FrameInterpolator::findFrames(KeyFrame& data, int64_t realElapsedTime) {
    FrameContext context;
    auto frames = data.getFrames();

    // 如果 elapsedTime 为负值，返回第一帧
    if (realElapsedTime < 0) {
        auto& bData = *frames.begin();
        return { bData,bData,(uint64_t)(-1),bData.duration,false,false };
    }

    // 使用 startTime 实现二分法
    auto it = std::lower_bound(frames.begin(), frames.end(), realElapsedTime,
        [](const KeyFrame::Point& point, int64_t time) {
            return point.startTime + point.duration <= time;
        });

    // 如果没有找到（realElapsedTime 超出范围），返回最后一帧
    if (it == frames.end()) {
        const auto& rData = *frames.rbegin();
        return { rData, rData, context.accumulatedTime, rData.duration, true };
    }

    // 确定当前帧
    size_t index = std::distance(frames.begin(), it);
    const auto& currentFrame = frames[index];
    const auto& nextFrame = (index + 1 < frames.size()) ? frames[index + 1] : currentFrame;

    context.before = currentFrame;
    context.after = nextFrame;
    context.accumulatedTime = currentFrame.startTime + currentFrame.duration;
    context.inDurationTime = realElapsedTime - currentFrame.startTime;

    return context;
    /*
    // 线性遍历所有帧
    for (size_t i = 0; i < frames.size(); i++) {
        context.accumulatedTime += frames[i].duration;

        if (context.accumulatedTime >= realElapsedTime) {
            context.before = frames[i];
            context.after = (i + 1 < frames.size()) ? frames[i + 1] : frames[i];
            //int64_t minus = (i > 0) ? frames[i - 1].duration : 0;
            context.inDurationTime = realElapsedTime - (context.accumulatedTime - frames[i].duration);
            return context;
        }
    }

    auto& rData = *frames.rbegin();
    return { rData,rData,context.accumulatedTime,rData.duration,true };
    */
}
//KeyFrame::Point FrameInterpolator::calculate(FrameContext context) {
//    KeyFrame::Point point = {};
//    double t;
//    if (context.before.duration <= 0) {
//        t = 1.0;
//    }
//    else {
//        t = (double)context.inDurationTime / (double)context.before.duration;
//        t = context.before.easing(t);
//    }
//    point.value = ((context.after.value - context.before.value) * t) + context.before.value;
//    return point;
//}
KeyFrame::Point FrameInterpolator::calculate(KeyFrame::Point start, KeyFrame::Point end,uint64_t inDurationTime,uint64_t duration,KeyFrame::Easing easing) {
    KeyFrame::Point point = {};
    double t;
    if (duration <= 0) {
        t = 1.0;
    }
    else {
        t = (double)inDurationTime / (double)duration;
        t = easing(t);
    }
    point.value = ((end.value - start.value) * t) + start.value;
    return point;
}
KeyFramePoints FrameInterpolator::Interpolate(KeyFrameLists& data, int64_t elapsedTime) {
    KeyFramePoints interpolatedData;
    if (data.empty()) {
        return interpolatedData; // 如果没有帧数据，直接返回
    }

    // 遍历帧数据
    for (auto& [name, keyFrame] : data) {
        int64_t realElapsedTime = elapsedTime - keyFrame.getData().offset;
        FrameContext context = findFrames(keyFrame, realElapsedTime);
        interpolatedData.insert(std::make_pair(name, calculate(context.before, context.after,
            context.inDurationTime, context.before.duration, context.before.easing
        )));
    }

    return interpolatedData;
}
KeyFramePoints FrameInterpolator::Interpolate(KeyFramePoints start, KeyFramePoints end, int64_t elapsedTime, uint64_t duration, int64_t offset, KeyFrame::Easing easing) {
    KeyFramePoints interpolatedData;
    if (start.size() != end.size()) throw std::invalid_argument("Size mismatched.");

    auto comparePairs = [](const std::pair<std::string, KeyFrame::Point>& a, const std::pair<std::string, KeyFrame::Point>& b) {
        return a.first < b.first;  // 按照 `std::string` 的字典顺序排序
    };

    std::vector<std::pair<std::string, KeyFrame::Point>> vecStart(start.begin(), start.end());
    std::vector<std::pair<std::string, KeyFrame::Point>> vecEnd(end.begin(), end.end());

    std::sort(vecStart.begin(), vecStart.end(), comparePairs);
    std::sort(vecEnd.begin(), vecEnd.end(), comparePairs);

    int64_t realElapsedTime = elapsedTime - offset;
    if (realElapsedTime < 0) {
        interpolatedData.insert(std::make_pair(vecStart.begin()->first, vecStart.begin()->second));
        return interpolatedData;
    }

    size_t size = vecStart.size();
    for (size_t i = 0; i < size; i++) {
        if (vecStart[i].first != vecEnd[i].first) throw std::runtime_error("FrameSign mismatched!!!");
        interpolatedData.insert(std::make_pair(vecStart[i].first, calculate(
            vecStart[i].second, vecEnd[i].second, realElapsedTime, duration, easing
        )));
    }
    return interpolatedData;
}