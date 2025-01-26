#pragma once
#include <functional>
#include <unordered_map>
#include <string>
#include <stdexcept>
class StateMachine {
private:
	std::unordered_map<std::string, bool> states;
public:
    // 设置状态
    void set(const std::string& state, bool value) {
        if (states[state] != value) { // 只有状态变化时才触发
            states[state] = value;
        }
    }

    void emplace(const std::string& state, bool value) {
        states.emplace(state, value);
    }

    // 获取状态值
    bool& get(const std::string& state) {
        if (has(state)) return states.at(state);
        throw std::invalid_argument("State (" + state + ") doesn't exist");
    }

    // 获取状态值，提供默认值
    bool getOrDefault(const std::string state, bool& defaultValue) const {
        return has(state) ? states.at(state) : defaultValue;
    }

    // 批量设置状态
    void putBatches(const std::unordered_map<std::string, bool>& stateMap) {
        for (auto& entry : stateMap) {
            states[entry.first] = entry.second; // 强制覆盖
        }
    }

    void putDefaults(const std::unordered_map<std::string, bool>& stateMap) {
        for (auto& entry : stateMap) {
            states.emplace(entry);
        }
    }

    // 切换状态（true -> false 或 false -> true）
    void toggle(const std::string& state) {
        set(state, !get(state));
    }

    // 清除所有状态
    void clear() {
        states.clear();
    }

    // 移除状态
    void erase(const std::string& state) {
        states.erase(state);
    }

    // 检查状态是否存在
    bool has(const std::string& state) const {
        return states.count(state) > 0;
    }

    bool& operator[](const std::string& state) {
        return get(state);
    }
};

class StateTrigger {
private:
    std::unordered_map <std::string, std::function<void(bool)>> triggers;
public:
    // 添加触发器
    void addTrigger(const std::string& triggerState, std::function<void(bool)> trigger) {
        triggers[triggerState] = std::move(trigger);
    }

    // 移除触发器
    void removeTrigger(const std::string& triggerState) {
        triggers.erase(triggerState);
    }

    // 运行当前状态的触发器
    void run(const std::string& state,bool condition) {
        if (!exist(state)) {
            throw std::runtime_error("No trigger defined for state (" + state + ")");
        }
        triggers[state](condition);
    }

    bool exist(const std::string& triggerState) {
        return triggers.count(triggerState) > 0;
    }
};