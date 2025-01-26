#pragma once
#include <unordered_map>
#include <string>
#include <any>
#include <functional>
#include <stdexcept>

class Trigger {
private:
    struct TriggerData {
        bool triggered = false; // �Ƿ��Ѵ���
        std::shared_ptr<void> data;          // �洢�����ݣ�֧���������ͣ�
    };

    std::unordered_map<std::string, TriggerData> triggerStates; // ������״̬�洢
    std::unordered_map<std::string, std::function<void(std::shared_ptr<void>)>> triggers; // �������߼�

public:
    // ��Ӵ�����
    void addTrigger(const std::string& triggerName, std::function<void(std::shared_ptr<void>)> trigger) {
        triggers[triggerName] = std::move(trigger);
        triggerStates.emplace(triggerName, TriggerData());
    }

    // �Ƴ�������
    void removeTrigger(const std::string& triggerName) {
        triggers.erase(triggerName);
        triggerStates.erase(triggerName);
    }

    // �������ݲ���Ǵ�����Ϊ���Ѵ�����
    template <typename T>
    void send(const std::string& triggerName, const T& value) {
        if (!existTrigger(triggerName)) throw std::invalid_argument("Trigger (named " + triggerName + ") doesn't exist");
        auto& state = triggerStates[triggerName];
        state.triggered = true;
        state.data = std::make_shared<T>(value);
    }

    // ��Ⲣ���д����������δ�����
    void process(const std::string& triggerName) {
        if (existTriggerState(triggerName)) {
            auto& state = triggerStates[triggerName];
            if (state.triggered) {
                triggers[triggerName](state.data); // ���д������߼�
                state.triggered = false;
            }
        }
    }

    void forEach() {
        for (auto& entry : triggers) {
            process(entry.first);
        }
    }

    // ��鴥�����Ƿ����
    bool existTrigger(const std::string& triggerName) const {
        return triggers.count(triggerName) > 0;
    }

    bool existTriggerState(const std::string& triggerStateName) const {
        return triggerStates.count(triggerStateName) > 0;
    }
};