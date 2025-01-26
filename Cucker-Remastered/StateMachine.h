#pragma once
#include <functional>
#include <unordered_map>
#include <string>
#include <stdexcept>
class StateMachine {
private:
	std::unordered_map<std::string, bool> states;
public:
    // ����״̬
    void set(const std::string& state, bool value) {
        if (states[state] != value) { // ֻ��״̬�仯ʱ�Ŵ���
            states[state] = value;
        }
    }

    void emplace(const std::string& state, bool value) {
        states.emplace(state, value);
    }

    // ��ȡ״ֵ̬
    bool& get(const std::string& state) {
        if (has(state)) return states.at(state);
        throw std::invalid_argument("State (" + state + ") doesn't exist");
    }

    // ��ȡ״ֵ̬���ṩĬ��ֵ
    bool getOrDefault(const std::string state, bool& defaultValue) const {
        return has(state) ? states.at(state) : defaultValue;
    }

    // ��������״̬
    void putBatches(const std::unordered_map<std::string, bool>& stateMap) {
        for (auto& entry : stateMap) {
            states[entry.first] = entry.second; // ǿ�Ƹ���
        }
    }

    void putDefaults(const std::unordered_map<std::string, bool>& stateMap) {
        for (auto& entry : stateMap) {
            states.emplace(entry);
        }
    }

    // �л�״̬��true -> false �� false -> true��
    void toggle(const std::string& state) {
        set(state, !get(state));
    }

    // �������״̬
    void clear() {
        states.clear();
    }

    // �Ƴ�״̬
    void erase(const std::string& state) {
        states.erase(state);
    }

    // ���״̬�Ƿ����
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
    // ��Ӵ�����
    void addTrigger(const std::string& triggerState, std::function<void(bool)> trigger) {
        triggers[triggerState] = std::move(trigger);
    }

    // �Ƴ�������
    void removeTrigger(const std::string& triggerState) {
        triggers.erase(triggerState);
    }

    // ���е�ǰ״̬�Ĵ�����
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