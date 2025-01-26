#include "Animator.h"
/*
TimeAxis& Animator::addTimeAxis(std::string name) {
	timeAxes.insert(std::make_pair(name, std::make_shared<TimeAxis>(TimeAxis())));
	currentTimeAxis = timeAxes[name];
	return *timeAxes[name];
}
void Animator::select(std::string name) {
	if (!exist(name)) throw std::runtime_error("Timeaxis (named " + name + ") doesn't exist");
	currentTimeAxis = timeAxes[name];
}
void Animator::erase(std::string name) {
	if (exist(name)) {
		if (currentTimeAxis == timeAxes[name])
			currentTimeAxis = nullptr;
		timeAxes.erase(name);
	}
}
void Animator::clear() {
	timeAxes.clear();
	currentTimeAxis = nullptr;
}

TimeAxis& Animator::getTimeAxis(const std::string& name) {
	if (!exist(name)) throw std::runtime_error("Timeaxis (named " + name + ") doesn't exist");
	return *timeAxes[name];
}
bool Animator::exist(std::string name) {
	return timeAxes.count(name) > 0;
}
TimeAxis* Animator::operator->() {
	if (!currentTimeAxis) {
		throw std::runtime_error("No timeaxis selected!");
	}
	return currentTimeAxis.get(); // ∑µªÿ÷∏’Î
}
TimeAxis& Animator::operator[](const std::string& name) {
	return getTimeAxis(name);
}
*/