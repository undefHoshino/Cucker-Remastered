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
            func(io);  // 将返回值传递给 io
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
        // 可扩展更多信号
    };

    enum ThreadPriority {
        System,  // 系统线程，不能被移除
        User     // 用户线程，可以添加和移除
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
        std::shared_ptr<void> data;         // 线程数据
    public:
        ThreadData() = default;
        ThreadData(std::chrono::duration<milliseconds> timeout, std::shared_ptr<void> data): data(data) {};
    };
public:
    using ThreadFunc = std::function<void(std::shared_ptr<void>, std::shared_ptr<ThreadHandler>)>;  // 线程函数类型
private:
    std::unordered_map<std::string, std::pair<ThreadFunc, ThreadPriority>> threads;  // 线程名称 -> (线程函数, 类型)
    std::unordered_map<std::string, std::shared_ptr<ThreadData>> threadData;    // 线程名称 -> 线程数据
    std::mutex mtx;  // 保护线程容器的锁
    std::atomic<bool> running{ true };  // 总控制开关，控制线程是否继续工作

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
    // 停止线程
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
    // 添加一个系统线程，系统线程不能被删除
    void addSystemThread(const std::string& name, ThreadFunc func, std::shared_ptr<ThreadData> data, std::shared_ptr<ThreadHandler> handler) {
        addThread(name, func, data, System);
    }
    // 运行线程的函数
    void runThread(const std::string& name, std::shared_ptr<ThreadData> data, ThreadFunc func) {
        try {
            while (running) {
                // 根据信号做相应的处理
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