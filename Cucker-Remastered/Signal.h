#pragma once
#include <vector>
#include <functional>
class SignalManager {
private:
	using Container = std::vector<std::pair<int, void*>>;
	Container signals;
public:
	void push(int sign,void* args) {
		signals.push_back(std::make_pair(sign, args));
	}
	Container get() {
		return signals;
	}
	void list(std::function<bool(std::pair<int, void*>)> func) {
		for (auto& ptr : signals) {
			if (!func(ptr)) break;
		}
	}
	bool empty() {
		return signals.empty();
	}
	void clear() {
		signals.clear();
	}
	void shrink() {
		signals.shrink_to_fit();
	}
};
