#pragma once
#include "Timeline.h"

class Transition : public IPlayback {
public:
	using Drawer = FrameDrawer<KeyFrameLoader>;
	struct TransitionParams {
		KeyFramePoints Start;
		KeyFramePoints End;
		uint64_t Duration = 0;
		int64_t Offset = 0;
		KeyFrame::Easing Easing = [](double t)->double { return t; };
	};
private:
	Drawer Framedrawer;
	TransitionParams Params;
public:
	void SetTransition(KeyFramePoints start, KeyFramePoints end, uint64_t duration,int64_t offset = 0, KeyFrame::Easing easing = [](double t)->double { return t; }) {
		Params = { start,end,duration,offset,easing };
		Clock.setMaxDuration<std::chrono::milliseconds>(duration + offset);
	}
	void SetDrawer(Drawer::DrawFunction* ptr, Drawer::DrawFunction make = Drawer::DrawFunction()) {
		if (ptr) {
			Framedrawer.Redirect(ptr);
			return;
		}
		Framedrawer.Make(make);
	}
	KeyFramePoints LoadFrameData() {
		return FrameInterpolator::Interpolate(Params.Start, Params.End, getElapsedTime<std::chrono::milliseconds>(), Params.Duration,Params.Offset, Params.Easing);
	}
	void Draw(ScreenA* screen, CanvasA* canvas, void* args) {
		KeyFrameLoader loader(LoadFrameData());
		Framedrawer.Draw(screen, canvas, loader, args);
	}
	Drawer& getDrawer() {
		return Framedrawer;
	}
};


//struct Transition {
//	enum FramePosition {
//		Start,
//		Current,
//		Location,
//		End
//	};
//	struct FrameLocator {
//		FramePosition position = FramePosition::Start;
//		int64_t tick = 0;
//		unsigned int frame = 0;
//	};
//	struct PlaybackSettings {
//		int64_t duration = 0;
//		int64_t offset = 0;
//		double speed = 1.0;
//		bool reversed = false;
//	};
//	struct Data {
//		UINT timelineID;
//		FrameLocator startFrame;
//		FrameLocator endFrame;
//		TimelinePlayback playback;
//		PlaybackSettings setting;
//	};
//private:
//	Data data;
//public:
//	Transition& setNode(int toTimelineID) {
//		data.timelineID = toTimelineID;
//		return *this;
//	}
//	Transition& setFrameStart(FramePosition position, int64_t tick = 0, unsigned int frame = 0) {
//		data.startFrame.position = position;
//		data.startFrame.tick = tick;
//		data.startFrame.frame = frame;
//		return *this;
//	}
//	Transition& setFrameEnd(FramePosition position, int64_t tick = 0, unsigned int frame = 0) {
//		data.endFrame.position = position;
//		data.endFrame.tick = tick;
//		data.endFrame.frame = frame;
//		return *this;
//	}
//	Transition& setPlayback(FrameDrawer func, int64_t duration, int64_t offset = 0, bool reversed = false, double speed = 1.0) {
//		data.playback.setDrawer(func);
//		data.setting.duration = duration;
//		data.setting.offset = offset;
//		data.setting.reversed = reversed;
//		data.setting.speed = speed;
//		return *this;
//	}
//	Data& getData() {
//		return data;
//	}
//};
//
//class TransitionManager {
//private:
//	std::map<UINT, Transition> transitions;
//public:
//	Transition& addTransition(UINT source, UINT copied = -1) {
//		if (copied != UINT(-1)) {
//			if (!exist(copied)) throw std::runtime_error("Transition doesn't exist");
//			transitions.emplace(source, transitions[copied]);
//			return transitions[source];
//		}
//		transitions.insert(std::make_pair(source, Transition()));
//		return transitions[source];
//	}
//	KeyFrame::FrameData getchFrameData(Transition::FrameLocator data, TimelinePlayback& timeline) {
//		using FramePosition = Transition::FramePosition;
//		KeyFrame::FrameData FrameData;
//		switch (data.position) {
//		case FramePosition::Start:
//			return timeline.getFrames().begin()->second.getData();
//		case FramePosition::Current:
//			if (timeline.isEnded())
//				return timeline.getFrames().rbegin()->second.getData();
//			return FrameInterpolator::Interpolate(timeline.getFrames(),timeline.getElapsedTime());
//		case FramePosition::End:
//			return timeline.getFrames().rbegin()->second.getData();
//		case FramePosition::Location:
//		{
//			if (data.frame != -1) {
//				auto& frames = timeline.getFrames();
//				if (frames.count(data.frame))
//					throw std::runtime_error("During getching FrameData: Frame does not exist");
//				return frames[data.frame].getData();
//			}
//			// Unsupported tick
//			return {};
//		}
//		default:
//			throw std::runtime_error("During getching FrameData: Invaild PathMode");
//		}
//	}
//	void setupTransition(UINT tranId, TimelinePlayback& start, TimelinePlayback& end) {
//		if (!exist(tranId)) {
//			return;
//		}
//		if (start.isEmpty()) throw std::runtime_error("During creating transition timeline: StartTimeline is empty");
//		if (end.isEmpty()) throw std::runtime_error("During creating transition timeline: EndTimeline is empty");
//		auto& tranData = transitions[tranId].getData();
//		KeyFrame::FrameData startFrame;
//		KeyFrame::FrameData endFrame;
//
//		startFrame = getchFrameData(tranData.startFrame, start);
//		endFrame = getchFrameData(tranData.endFrame, end);
//
//		tranData.playback.clear();
//		tranData.playback.addframe(0).receiveData(startFrame).setTick(tranData.setting.duration);
//		tranData.playback.addframe(1).receiveData(endFrame);
//		tranData.playback.ConfigurePlayback(tranData.setting.offset, TimelinePlayback::PlayMode::End);
//		tranData.playback.setupFrames();
//
//	}
//	Transition::PlaybackSettings getNextPlayback(UINT tranId) {
//		if (!exist(tranId)) {
//			return {};
//		}
//		return transitions[tranId].getData().setting;
//	}
//	bool playTransition(UINT tranId) {
//		if (!exist(tranId)) return false;
//		auto& tranData = transitions[tranId].getData();
//		if (tranData.playback.isEmpty()) throw std::runtime_error("During playing transition timeline: timeline is empty");
//		tranData.playback.Play();
//		return true;
//	}
//	bool transition(ScreenA* screen, CanvasA* canvas,UINT tranId) {
//		if (!exist(tranId)) return false;
//
//		auto& tranData = transitions[tranId].getData();
//		if (tranData.playback.isEnded()) return false;
//
//		tranData.playback.DrawFrame(screen,canvas);
//		return true;
//	}
//	bool exist(UINT id) {
//		return transitions.count(id) > 0;
//	}
//};