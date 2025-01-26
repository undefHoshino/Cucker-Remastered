#pragma once
#include <map>
#include <vector>
#include <functional>
#include <chrono>
#include "StateMachine.h"
#include "Trigger.h"
#include "Transition.h"

class Animator {
public:
	struct SegmentArgs {
		int64_t duration;
		double value;
		KeyFrame::Easing easing = [](double t)->double { return t; };
	};

	struct TriggerArgs {
		std::string name;
		std::function<void(std::shared_ptr<void>)> func;
	};
	template<class T>
	struct TriggerSendArgs {
		std::string name;
		T data;
	};
	struct PlaybackActionPreset {
		using PlaybackFunction = std::function<void(std::shared_ptr<void>)>;
		//bool
		static PlaybackFunction InterruptiblePlayback(Timeline& timeline, StateMachine& stateMachine, std::string symbolName) {
			std::string flagName = "__FlagPlayback" + symbolName;
			stateMachine.emplace(flagName, false);
			return [&timeline, &stateMachine, flagName](std::shared_ptr<void> args) {
				bool flag = *std::static_pointer_cast<bool>(args);
				bool& order = stateMachine[flagName];
				if (flag) {
					if (!order) {
						if (timeline.isEnded() || timeline.isPaused()) {
							timeline.Reset();
						}
						else {
							timeline.Reverse(); //Reversed to Start ->
						}
						order = true; // Start ->
					}
				}
				else {
					if (order) {
						if (timeline.isEnded()) {
							timeline.SyncToMaxDuration();
						}
						timeline.Reverse();
						order = false; // <- End
					}
				}
			};
		}
		//bool
		static PlaybackFunction LoopingPlayback(TimeAxis& timeaxis, Timeline& timeline, std::string timelineName) {
			return [timelineName, &timeaxis, &timeline](std::shared_ptr<void> args) {
				bool flag = *std::static_pointer_cast<bool>(args);
				if (flag) {
					if (timeline.isEnded()) {
						timeline.Reset();
					}
					if (timeline.isPaused()) {
						timeaxis.Enqueue(timelineName);
						timeline.Play();
					}
				}
				else {
					timeaxis.Dequeue(timelineName);
					timeline.Stop();
				}
			};
		}
		//bool
		static PlaybackFunction DirectPlayBack(TimeAxis& timeaxis, Timeline& timeline, std::string timelineName) {
			return [&timeline, &timeaxis, timelineName](std::shared_ptr<void> args) {
				bool flag = *std::static_pointer_cast<bool>(args);
				if (flag) {
					if (timeline.isPaused()) {
						timeaxis.Enqueue(timelineName);
						timeline.Play();
					}
				}
				else {
					timeaxis.Dequeue(timelineName);
					timeline.Stop();
				}
			};
		}
	};
	using AxisDrawer = TimeAxis::Drawer::DrawFunction;
	using LineDrawer = Timeline::Drawer::DrawFunction;
public:
	TimeAxis timeAxis;
	StateMachine stateMachine;
	Trigger trigger;

	Animator() {};
	/*
	void ManageTimeAxes(std::string selected = "", std::vector<std::string> pushed = {}, bool pushCover = true, std::vector<std::string> erased = {}) {
		for (auto& name : pushed) {
			if (!parent->exist(name) || pushCover)
				parent->addTimeAxis(name);
		}
		for (auto& name : erased) {
			parent->erase(name);
		}
		if (selected != "")
			parent->select(selected);
	}
	*/
	void SetDrawer(AxisDrawer* ptrDrawfunc, AxisDrawer makeDrawfunc) {
		timeAxis.SetDrawer(ptrDrawfunc, makeDrawfunc);
	}
	void SetDrawer(std::string timeline, LineDrawer* ptrDrawfunc, LineDrawer makeDrawfunc) {
		if (!timeAxis.existTimeline(timeline)) throw std::invalid_argument("Timeline (named " + timeline + ") doesn't exist");
		timeAxis.getTimeline(timeline).SetDrawer(ptrDrawfunc, makeDrawfunc);
	}
	void DrawQueue(std::vector<std::string> enqueued = {}, std::vector<std::string> dequeued = {}) {
		for (auto& name : enqueued) {
			timeAxis.Enqueue(name);
		}
		for (auto& name : dequeued) {
			timeAxis.Dequeue(name);
		}
	}
	void Initialize() {
		timeAxis.Initialize();
	}
	Timeline& ConfigureTimeline(std::string name, std::string SegmentName = "", std::vector<SegmentArgs> Segments = {}) {
		if (!timeAxis.existTimeline(name))
			timeAxis.addTimeline(name);
		auto& timeline = timeAxis.getTimeline(name);
		if (SegmentName.empty()) return timeline;

		auto& keyframe = timeline.addSegment(SegmentName);
		for (auto& args : Segments) {
			keyframe.push(args.duration, args.value, args.easing);
		}
		return timeline;
	}
	template<class T = bool>
	void ManageTriggers(std::vector<TriggerSendArgs<T>> sends = {}, std::vector<TriggerArgs> pushs = {}, std::vector<std::string> removed = {}) {
		for (auto& args : pushs) {
			trigger.addTrigger(args.name, args.func);
		}
		for (auto& args : sends) {
			trigger.send(args.name, args.data);
		}
		for (auto& name : removed) {
			trigger.removeTrigger(name);
		}
	}
	void putStates(std::unordered_map<std::string, bool> sets, bool emplace = false) {
		if (emplace) stateMachine.putDefaults(sets);
		else		 stateMachine.putBatches(sets);
	}
	void DrawEach(ScreenA* screen, CanvasA* canvas, void* args) {
		timeAxis.DrawEach(screen, canvas, args);
	}
	void DrawUnified(ScreenA* screen, CanvasA* canvas, void* args) {
		timeAxis.DrawUnified(screen, canvas, args);
	}
	Timeline& getTimeline(std::string name) {
		return timeAxis.getTimeline(name);
	}
	Trigger& getTrigger() {
		return trigger;
	}
	StateMachine& getStateMachine() {
		return stateMachine;
	}
	TimeAxis& getTimeAxis() {
		return timeAxis;
	}
	TimeAxis& operator()() {
		return timeAxis;
	}
};
/*
class Animator {
protected:
	std::unordered_map<std::string, std::shared_ptr<TimeAxis>> timeAxes;
	std::shared_ptr<TimeAxis> currentTimeAxis;
public:
	StateMachine stateMachine;
	Trigger trigger;

	Animator() :currentTimeAxis(nullptr) {};
	TimeAxis& addTimeAxis(std::string name);
	void erase(std::string name);
	void select(std::string name);
	void clear();
	TimeAxis& getTimeAxis(const std::string& name);
	bool exist(std::string name);

	TimeAxis* operator->();
	TimeAxis& operator[](const std::string& name);
};
*/