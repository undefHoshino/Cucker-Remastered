#pragma once
#include <map>
#include <vector>
#include "KeyFrame.h"

class IPlayback {
protected:
	HpetClock Clock;
public:
	virtual void Play();
	virtual void Play(int64_t offset, bool reversed, double speed);
	virtual void Reset();
	virtual void Stop();
	virtual void Pause();
	virtual void Resume();
	virtual void SetSpeed(double speed);
	virtual void Reverse();
	virtual void SyncToMaxDuration();
	virtual void Rollback(int64_t rollbackTime);
	virtual bool isPaused();
	virtual bool isEnded();
	virtual bool isReversed();
	virtual float getProgress();
	virtual double getSpeed();
	virtual HpetClock& getClock();
	template<class Duration = std::chrono::milliseconds>
	int64_t getElapsedTime() {
		return Clock.getElapsedTime<Duration>().count();
	}
	template<class Duration = std::chrono::milliseconds>
	int64_t getMaxDuration() {
		return Clock.getMaxDuration<Duration>().count();
	}
};

class KeyFrameLoader {
private:
	KeyFramePoints data;
public:
	KeyFrameLoader() = default;
	explicit KeyFrameLoader(KeyFramePoints points) : data(std::move(points)) {}

	void push(KeyFramePoints data);
	bool has(const std::string& name);
	double get(std::string name);
	double operator[](const std::string& name);
};

class KeyFrameGroupLoader {
private:
	std::unordered_map<std::string, KeyFrameLoader> loaders;
public:
	void addLoader(std::string name, KeyFramePoints data);
	bool hasLoader(const std::string& name);
	bool ifContains(const std::string& name, std::function<void(KeyFrameLoader&)>& func);
	KeyFrameLoader& getLoader(std::string name);
	KeyFrameLoader& operator[](const std::string& name);
};

class Timeline : public IPlayback {
public:
	using Drawer = FrameDrawer<KeyFrameLoader>;
protected:
	KeyFrameLists SegmentDatas;
	Drawer Framedrawer;
public:
	KeyFrame& addSegment(std::string segment);
	KeyFrame& getSegment(std::string segment);
	void Setup(int64_t maxClockMilliseconds = 0);
	KeyFramePoints LoadFrameData();

	void SetDrawer(Drawer::DrawFunction* ptr, Drawer::DrawFunction make = Drawer::DrawFunction());
	void Draw(ScreenA* screen, CanvasA* canvas, void* args);
	Drawer& getDrawer();
protected:
	bool existSegment(std::string segment);
};

class TimeAxis {
public:
	using Drawer = FrameDrawer<KeyFrameGroupLoader>;
private:
	Drawer unifiedDrawer;
	std::unordered_map<std::string, Timeline> timelines;
	std::vector<std::pair<std::string,Timeline*>> drawQueue;
public:
	Timeline& addTimeline(std::string name);
	Timeline& getTimeline(std::string name);
	bool existTimeline(std::string name);
	void Initialize();
	void Enqueue(std::string name);
	void Dequeue(std::string name);
	std::unordered_map<std::string, Timeline>& data();

	void DrawUnified(ScreenA* screen, CanvasA* canvas, void* args);
	void DrawEach(ScreenA* screen, CanvasA* canvas, void* args);
	void SetDrawer(Drawer::DrawFunction* ptr, Drawer::DrawFunction make);
};
