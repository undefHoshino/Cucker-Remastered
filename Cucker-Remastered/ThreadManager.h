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

class ThreadManager {
public:
    enum struct ThreadPriority {
        System,  
        User   
    };

    struct ThreadHandler {
    protected:
        bool exit = false;
        std::promise<int> return_value;
        std::chrono::steady_clock::duration timeout_duration = std::chrono::seconds(5);

        friend class ThreadManager;

        virtual void Start() {};
        virtual void Return(int val) {
            return_value.set_value(val);
            exit = true;
        }
    public:
        virtual void Stop() {
            exit = true;
        }
        virtual bool shouldStop() {
            return exit;
        }
        std::future<int> getFuture() {
            return return_value.get_future();
        }

        template<class _Duration = std::chrono::milliseconds>
        void SetTimeoutDuration(int64_t duration) {
            timeout_duration = _Duration(duration);
        }
        template<class _Duration = std::chrono::milliseconds>
        _Duration getTimeoutDuration() {
            return std::chrono::duration_cast<_Duration>(timeout_duration);
        }
    };
public:
    using ThreadFunc = std::function<int(ThreadHandler&)>;  // 线程函数类型
    typedef struct {
        ThreadFunc func;
        ThreadPriority priority;
        ThreadHandler handler;
    } ThreadObject;
private:
    std::unordered_map<std::string, ThreadObject> threads;  // 线程名称 -> (线程函数, 类型)
    std::mutex mtx;  // 保护线程容器的锁
    Logger logger;
public:
    ThreadManager() :logger("ThreadManager") {};

    template<class _ThreadHandler>
    void addUserThread(const std::string& name, ThreadFunc func) {
        addThread<_ThreadHandler>(name, func, ThreadPriority::User);
    }
    void stopUserThread(const std::string& name);
    bool hasThread(const std::string& name);
    void checkUserThread(const std::string& name);

    void Stop();
protected:
    template<class _ThreadHandler>
    void addThread(const std::string& name, ThreadFunc func, ThreadPriority priority) {
        std::lock_guard<std::mutex> lock(mtx);
        if (!hasThread(name)) {
            threads.insert(std::make_pair(name, ThreadObject(func, priority, _ThreadHandler())));

            std::thread([this, name]() { /// undefined
                auto& object = threads[name];
                object.handler.Start();

                this->runThread(name, object);
            }).detach();
        }
    }
    // 添加一个系统线程，系统线程不能被删除
    template<class _ThreadHandler>
    void addSystemThread(const std::string& name, ThreadFunc func) {
        addThread<_ThreadHandler>(name, func, ThreadPriority::System);
    }

    void stopThread(const std::string& name);
    void runThread(const std::string& name, ThreadObject& object);
};
