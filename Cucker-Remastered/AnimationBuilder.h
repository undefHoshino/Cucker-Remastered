#pragma once
#include "Animator.h"

/*
class AnimationBuilder {
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
			return [&timeline,&stateMachine, flagName](std::shared_ptr<void> args) {
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
		static PlaybackFunction LoopingPlayback(Animator& animator, Timeline& timeline, std::string timelineName, std::string axisName = "") {
			auto& axis = axisName.empty() ? *animator.operator->() : animator[axisName];
			return [timelineName, &axis, &timeline](std::shared_ptr<void> args) {
				bool flag = *std::static_pointer_cast<bool>(args);
				if (flag) {
					if (timeline.isEnded()) {
						timeline.Reset();
					}
					if (timeline.isPaused()) {
						axis.Enqueue(timelineName);
						timeline.Play();
					}
				}
				else {
					axis.Dequeue(timelineName);
					timeline.Stop();
				}
			};
		}
		//bool
		static PlaybackFunction DirectPlayBack(Animator& animator, Timeline& timeline, std::string timelineName, std::string axisName = "") {
			auto& axis = axisName.empty() ? *animator.operator->() : animator[axisName];
			return [&timeline,&axis, timelineName](std::shared_ptr<void> args) {
				bool flag = *std::static_pointer_cast<bool>(args);
				if (flag) {
					if (timeline.isPaused()) {
						axis.Enqueue(timelineName);
						timeline.Play();
					}
				}
				else {
					axis.Dequeue(timelineName);
					timeline.Stop();
				}
			};
		}
	};
private:
	using AxisDrawer = TimeAxis::Drawer::DrawFunction;
	using LineDrawer = Timeline::Drawer::DrawFunction;
	Animator* parent = nullptr;
public:
	AnimationBuilder() {};
	AnimationBuilder(Animator* ptr) :parent(ptr) {};
	void redirect(Animator* ptr) {
		this->parent = ptr;
	}
	void ManageTimeAxes(std::string selected = "",std::vector<std::string> pushed = {}, bool pushCover = true, std::vector<std::string> erased = {}) {
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

	void SetDrawer(AxisDrawer* ptrDrawfunc, AxisDrawer& makeDrawfunc) {
		auto& axis = *parent->operator->();
		axis.SetDrawer(ptrDrawfunc, makeDrawfunc);
	}
	void SetDrawer(std::string timeline, LineDrawer* ptrDrawfunc, LineDrawer& makeDrawfunc) {
		auto& axis = *parent->operator->();
		if (!axis.existTimeline(timeline)) throw std::invalid_argument("Timeline (named " + timeline + ") doesn't exist");
		axis.getTimeline(timeline).SetDrawer(ptrDrawfunc, makeDrawfunc);
	}
	void DrawQueue(std::vector<std::string> enqueued = {}, std::vector<std::string> dequeued = {}) {
		auto& axis = *parent->operator->();
		for (auto& name : enqueued) {
			axis.Enqueue(name);
		}
		for (auto& name : dequeued) {
			axis.Dequeue(name);
		}
	}
	void Initialize() {
		parent->operator->()->Initialize();
	}
	Timeline& ConfigureTimeline(std::string name, std::string SegmentName, std::vector<SegmentArgs> Segments = {}) {
		auto& axis = *parent->operator->();
		if (!axis.existTimeline(name))
			axis.addTimeline(name);
		auto& timeline = axis.getTimeline(name);
		auto& keyframe = timeline.addSegment(SegmentName);
		for (auto& args : Segments) {
			keyframe.push(args.duration, args.value, args.easing);
		}
		return timeline;
	}
	template<class T = bool>
	void ManageTriggers(std::vector<TriggerSendArgs<T>> sends = {}, std::vector<TriggerArgs>pushs = {}, std::vector<std::string> removed = {}) {
		auto& trg = parent->trigger;
		for (auto& args : pushs) {
			trg.addTrigger(args.name, args.func);
		}
		for (auto& args : sends) {
			trg.send(args.name, args.data);
		}
		for (auto& name : removed) {
			trg.removeTrigger(name);
		}
	}
	void putStates(std::unordered_map<std::string, bool> sets,bool emplace = false) {
		auto& statem = parent->stateMachine;
		if (emplace) statem.putDefaults(sets);
		else		 statem.putBatches(sets);
	}
	Timeline& getTimeline(std::string name) {
		return parent->operator->()->getTimeline(name);
	}
	Trigger& getTrigger() {
		return parent->trigger;
	}
	StateMachine& getStateMachine() {
		return parent->stateMachine;
	}
	Animator* getParent() {
		return parent;
	}
};
*/