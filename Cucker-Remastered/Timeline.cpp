#include "Timeline.h"

void IPlayback::Play() {
	Clock.Start();
}
void IPlayback::Play(int64_t offset, bool reversed, double speed) {
	int64_t totalOffset = 0;
	if (reversed) {
		totalOffset = Clock.getMaxDuration().count() - offset;
	}
	else {
		totalOffset = offset;
	}
	Clock.Start();
	Clock.setDuration(totalOffset);
	Clock.SetSpeed(reversed ? -speed : speed);
}
void IPlayback::Reset() {
	Clock.Reset();
}
void IPlayback::Stop() {
	Clock.Reset();
	Clock.Pause();
	Clock.setDuration(0);
}
void IPlayback::Pause() {
	Clock.Pause();
}
void IPlayback::Resume() {
	Clock.Resume();
}
void IPlayback::SetSpeed(double speed) {
	Clock.SetSpeed(speed);
}
void IPlayback::Reverse() {
	Clock.Reverse();
}
void IPlayback::SyncToMaxDuration() {
	Clock.syncFromMaxDuration();
}
void IPlayback::Rollback(int64_t rollbackTime) {
	Clock.Rollback(rollbackTime);
}
bool IPlayback::isPaused() {
	return Clock.isPaused();
}
bool IPlayback::isEnded() {
	float progress = Clock.getProgress(false, true);
	return Clock.isReversed() ? progress <= 0.0 : progress >= 1.0;
}
bool IPlayback::isReversed() {
	return Clock.isReversed();
}
float IPlayback::getProgress() {
	return Clock.getProgress();
}
double IPlayback::getSpeed() {
	return Clock.getSpeed();
}
HpetClock& IPlayback::getClock() {
	return Clock;
}


double KeyFrameLoader::get(std::string name) {
	if (has(name)) {
		return data[name].value;
	}
	throw std::runtime_error("KeyFrame (named " + name + ") doesn't exist");
}
bool KeyFrameLoader::has(const std::string& name) {
	return data.count(name) > 0;
}
void KeyFrameLoader::push(KeyFramePoints data) {
	this->data = data;
}
double KeyFrameLoader::operator[](const std::string& name) {
	return get(name);
}

void KeyFrameGroupLoader::addLoader(std::string name, KeyFramePoints data) {
	loaders.insert(std::make_pair(name, KeyFrameLoader(data)));
}
KeyFrameLoader& KeyFrameGroupLoader::getLoader(std::string name) {
	if (hasLoader(name)) return loaders[name];
	throw std::runtime_error("KeyFrameGroupLoader: Timeline (named " + name + ") doesn't exist");
}
bool KeyFrameGroupLoader::hasLoader(const std::string& name) {
	return loaders.count(name) > 0;
}
bool KeyFrameGroupLoader::ifContains(const std::string& name, std::function<void(KeyFrameLoader&)>& func) {
	if (!hasLoader(name))
		return false;

	func(loaders[name]);
	return true;
}
KeyFrameLoader& KeyFrameGroupLoader::operator[](const std::string& name) {
	return getLoader(name);
}


KeyFrame& Timeline::addSegment(std::string segment) {
	SegmentDatas.emplace(segment, KeyFrame(0)); // New Segment
	return SegmentDatas[segment];
}
KeyFrame& Timeline::getSegment(std::string segment) {
	if (!existSegment(segment)) throw std::runtime_error("Segment doesn't exist");
	return SegmentDatas[segment];
}
bool Timeline::existSegment(std::string segment) {
	return SegmentDatas.count(segment) > 0;
}
void Timeline::Setup(int64_t maxClockMilliseconds) {
	if (maxClockMilliseconds != 0) {
		Clock.setMaxDuration(maxClockMilliseconds);
		return;
	}
	uint64_t longestDuration = 0;
	for (const auto& frame : SegmentDatas) {
		longestDuration = std::max(longestDuration, frame.second.calcTotalDuration());
	}
	Clock.setMaxDuration(longestDuration);
}
void Timeline::SetDrawer(Drawer::DrawFunction* ptr, Drawer::DrawFunction make) {
	if (ptr) {
		Framedrawer.Redirect(ptr);
		return;
	}
	Framedrawer.Make(make);
}
void Timeline::Draw(ScreenA* screen, CanvasA* canvas, void* args) {
	KeyFrameLoader loader(LoadFrameData());
	Framedrawer.Draw(screen, canvas, loader, args);
}
Timeline::Drawer& Timeline::getDrawer() {
	return Framedrawer;
}
KeyFramePoints Timeline::LoadFrameData() {
	return FrameInterpolator::Interpolate(SegmentDatas, getElapsedTime<std::chrono::milliseconds>());
}

Timeline& TimeAxis::addTimeline(std::string name) {
	timelines.emplace(name, Timeline()); // New Timeline
	return timelines[name];
}
Timeline& TimeAxis::getTimeline(std::string name) {
	if (!existTimeline(name)) throw std::runtime_error("Timeline (named " + name + ") doesn't exist");
	return timelines[name];
}
bool TimeAxis::existTimeline(std::string name) {
	return timelines.count(name) > 0;
}
void TimeAxis::SetDrawer(Drawer::DrawFunction* ptr, Drawer::DrawFunction make) {
	if (ptr) {
		unifiedDrawer.Redirect(ptr);
		return;
	}
	unifiedDrawer.Make(make);
}
void TimeAxis::DrawUnified(ScreenA* screen, CanvasA* canvas, void* args) {
	KeyFrameGroupLoader loader;
	for (auto& ptr : drawQueue) {
		loader.addLoader(ptr.first, ptr.second->LoadFrameData());
	}
	unifiedDrawer.Draw(screen, canvas, loader, args);
}
void TimeAxis::DrawEach(ScreenA* screen, CanvasA* canvas, void* args) {
	for (auto& ptr : timelines) {
		ptr.second.Draw(screen, canvas, args);
	}
}
void TimeAxis::Initialize() {
	for (auto& timeline : timelines) {
		timeline.second.Setup();
		timeline.second.Stop();
	}
}
void TimeAxis::Enqueue(std::string name) {
	if (!existTimeline(name)) throw std::runtime_error("Timeline (named " + name + ") doesn't exist");
	drawQueue.push_back(std::make_pair(name, &timelines[name]));
}
void TimeAxis::Dequeue(std::string name) {
	if (!existTimeline(name)) throw std::runtime_error("Timeline (named " + name + ") doesn't exist");

	auto it = std::remove_if(drawQueue.begin(), drawQueue.end(), [&name, this](const std::pair<std::string, Timeline*>& item) {
		return item.first == name; // Æ¥Åä name
		});

	if (it != drawQueue.end()) {
		drawQueue.erase(it, drawQueue.end()); // ÒÆ³ýËùÓÐÆ¥ÅäµÄÔªËØ
	}
}

std::unordered_map<std::string, Timeline>& TimeAxis::data() {
	return timelines;
}