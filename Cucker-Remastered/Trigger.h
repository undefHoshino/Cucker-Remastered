#pragma once
#include <unordered_map>
#include <string>
#include <any>
#include <functional>
#include <stdexcept>

class Trigger {
private:
    struct TriggerData {
        bool triggered = false; // 是否已触发
        std::shared_ptr<void> data;          // 存储的数据（支持任意类型）
    };

    std::unordered_map<std::string, TriggerData> triggerStates; // 触发器状态存储
    std::unordered_map<std::string, std::function<void(std::shared_ptr<void>)>> triggers; // 触发器逻辑

public:
    // 添加触发器
    void addTrigger(const std::string& triggerName, std::function<void(std::shared_ptr<void>)> trigger) {
        triggers[triggerName] = std::move(trigger);
        triggerStates.emplace(triggerName, TriggerData());
    }

    // 移除触发器
    void removeTrigger(const std::string& triggerName) {
        triggers.erase(triggerName);
        triggerStates.erase(triggerName);
    }

    // 发送数据并标记触发器为“已触发”
    template <typename T>
    void send(const std::string& triggerName, const T& value) {
        if (!existTrigger(triggerName)) throw std::invalid_argument("Trigger (named " + triggerName + ") doesn't exist");
        auto& state = triggerStates[triggerName];
        state.triggered = true;
        state.data = std::make_shared<T>(value);
    }

    // 检测并运行触发器（单次触发）
    void process(const std::string& triggerName) {
        if (existTriggerState(triggerName)) {
            auto& state = triggerStates[triggerName];
            if (state.triggered) {
                triggers[triggerName](state.data); // 运行触发器逻辑
                state.triggered = false;
            }
        }
    }

    void forEach() {
        for (auto& entry : triggers) {
            process(entry.first);
        }
    }

    // 检查触发器是否存在
    bool existTrigger(const std::string& triggerName) const {
        return triggers.count(triggerName) > 0;
    }

    bool existTriggerState(const std::string& triggerStateName) const {
        return triggerStates.count(triggerStateName) > 0;
    }
};