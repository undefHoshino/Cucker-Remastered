#pragma once
#include <mutex>
#include <atomic>
#include <thread>
#include <string>
#include <functional>
#include <unordered_map>
#include <condition_variable>
#include <future>
#include "Logger.h"
#include "Exceptions.h"
#include "ConsoleFunction.h"

class ConsoleThread {
public:
    enum Signal {
        None,
        Success,
        Failure,
        Exit
    };
    struct IOSignal {
        Signal in = None;
        Signal out = None;
    };
private:

    using _function = std::function<void(IOSignal*)>; // in - out
	std::unordered_map<std::string, std::thread>ThreadMap;

    struct {
        IOSignal renderSignal;
        IOSignal inputSignal;
    } signals;

public:

    void Create(_function render, _function inputWorker) {
        addThread("Render", render, &signals.renderSignal);
        addThread("InputWorker", inputWorker, &signals.inputSignal);
    }
    void Supervisor() {
        if (!CheckConsoleInstance()) {
            signals.renderSignal.in = Exit;
            signals.inputSignal.in = Exit;
            throw ReportException("Console Terminated.");
        }
    }
    void Remove() {
        removeThread("Render");
        removeThread("InputWorker");
    }

private:
    void addThread(const std::string& name, _function func, IOSignal* io) {
        ThreadMap[name] = std::thread([func = std::move(func), io]() mutable {
            func(io);  // ������ֵ���ݸ� io
        });
    }
    void removeThread(const std::string name) {
        auto& Thread = ThreadMap[name];
        if (exist(name)) {
            if (Thread.joinable()) {
                Thread.join();
            }
            ThreadMap.erase(name);
        }
    }
    bool exist(const std::string name) {
        return ThreadMap.count(name) > 0;
    }
};
/*
class ThreadManager {
public:
    enum Signal {
        None,
        Exit,
        Restart,
        // ����չ�����ź�
    };

    enum ThreadPriority {
        System,  // ϵͳ�̣߳����ܱ��Ƴ�
        User     // �û��̣߳�������Ӻ��Ƴ�
    };
protected:
    struct ThreadHandler {
    public:
        using Duration = std::chrono::duration<milliseconds>;
    private:
        Signal signal = Signal::None;
        Duration timeoutDuration;
    public:
        void setTimeout(Duration timeout) {
            this->timeoutDuration = timeout;
        }
        void stop() {
            this->signal = Signal::Exit;
        }
        void restart() {
            this->signal = Signal::Restart;
        }
        Duration getTimeoutDuration() {
            return timeoutDuration;
        }
        Signal getSignal() {
            return this->signal;
        }
    };
    struct ThreadData {
    private:
        friend class ThreadManager;
        std::shared_ptr<ThreadHandler> handler;
        std::shared_ptr<void> data;         // �߳�����
    public:
        ThreadData() = default;
        ThreadData(std::chrono::duration<milliseconds> timeout, std::shared_ptr<void> data): data(data) {};
    };
public:
    using ThreadFunc = std::function<void(std::shared_ptr<void>, std::shared_ptr<ThreadHandler>)>;  // �̺߳�������
private:
    std::unordered_map<std::string, std::pair<ThreadFunc, ThreadPriority>> threads;  // �߳����� -> (�̺߳���, ����)
    std::unordered_map<std::string, std::shared_ptr<ThreadData>> threadData;    // �߳����� -> �߳�����
    std::mutex mtx;  // �����߳���������
    std::atomic<bool> running{ true };  // �ܿ��ƿ��أ������߳��Ƿ��������

public:
    std::shared_ptr<ThreadData> newThreadData(std::shared_ptr<void> data) {
        return std::make_shared<ThreadData>(data);
    }
    void addUserThread(const std::string& name, ThreadFunc func, std::shared_ptr<ThreadData> data) {
        addThread(name, func, data, User);
    }
    void stopUserThread(const std::string& name) {
        checkUserThread(name);
        stopThread(name);
    }
    void restartUserThread(const std::string& name) {
        checkUserThread(name);
        restartThread(name);
    }
    bool hasThread(const std::string& name) {
        return threads.find(name) != threads.end();
    }
    void checkUserThread(const std::string& name) {
        if (!hasThread(name)) throw std::invalid_argument("Thread (named " + name + " doesn't exist");
        if (threads[name].second == System) {
            throw std::runtime_error("Access denied.");
        }
    }
protected:
    void addThread(const std::string& name, ThreadFunc func, std::shared_ptr<ThreadData> data, ThreadPriority priority) {
        std::lock_guard<std::mutex> lock(mtx);
        if (!hasThread(name)) {
            threads[name] = { func, priority };
            threadData[name] = data;
            std::thread([this, name, data, func]() { /// undefined
                this->runThread(name, data, func);
            }).detach();
        }
    }
    // ֹͣ�߳�
    void stopThread(const std::string& name) {
        std::lock_guard<std::mutex> lock(mtx);
        if (hasThread(name)) {
            threadData[name]->handler->stop();
            threads.erase(name);
            threadData.erase(name);
        }
    }
    void restartThread(const std::string& name, std::shared_ptr<ThreadData> newData = nullptr) {
        std::lock_guard<std::mutex> lock(mtx);
        if (hasThread(name)) {
            threadData[name]->handler->restart();
            auto&& data = threadData[name]->data;
            auto&& func = threads[name].first;
            auto&& priority = threads[name].second;
            threads.erase(name);
            threadData.erase(name);

            std::shared_ptr<ThreadData> newThreadData;
            if (newData) {
                newThreadData = std::make_shared<ThreadData>(newData);
            }
            else {
                newThreadData = std::make_shared<ThreadData>(data);
            }

            addThread(name, func, newThreadData, priority);
        }
    }
    // ���һ��ϵͳ�̣߳�ϵͳ�̲߳��ܱ�ɾ��
    void addSystemThread(const std::string& name, ThreadFunc func, std::shared_ptr<ThreadData> data, std::shared_ptr<ThreadHandler> handler) {
        addThread(name, func, data, System);
    }
    // �����̵߳ĺ���
    void runThread(const std::string& name, std::shared_ptr<ThreadData> data, ThreadFunc func) {
        try {
            while (running) {
                // �����ź�����Ӧ�Ĵ���
                if (data->handler->getSignal()) {
                    break;
                }
                auto future = std::async(std::launch::async, func, data->data, data->handler);
                if (future.wait_for(data->handler->getTimeoutDuration()) == std::future_status::timeout) {
                    Logger::GetInstance().error("Thread ", name, " timed out after ", data->handler->getTimeoutDuration().count(), "ms.");
                }
            }
        }
        catch (const std::exception& e) {
            Logger::GetInstance().error("Thread ", name, " encountered an error: ", e.what());
        }
        data->handler->stop();
    }
    bool checkSignal(std::atomic<Signal>& signal) {
        return !(signal == Exit || signal == Restart);
    }
};
*/