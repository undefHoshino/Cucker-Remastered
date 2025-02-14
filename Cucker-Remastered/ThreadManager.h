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

/*
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
*/

class ThreadManager {
public:
    enum struct Signal {
        None,
        Exit,
    };

    enum struct ThreadPriority {
        System,  
        User   
    };
protected:
    struct ThreadHandler {
    protected:
        Signal signal = Signal::None;
    public:
        virtual void Start() {}

        virtual void Stop() {
            this->signal = Signal::Exit;
        }

        Signal getSignal() {
            return this->signal;
        }
    };
public:
    using ThreadFunc = std::function<void(std::shared_ptr<ThreadHandler>)>;  // �̺߳�������
    typedef struct {
        ThreadFunc func;
        ThreadPriority priority;
        std::shared_ptr<ThreadHandler> handler;
    } ThreadObject;
private:
    std::unordered_map<std::string, ThreadObject> threads;  // �߳����� -> (�̺߳���, ����)
    std::mutex mtx;  // �����߳���������
    Logger logger;
public:
    ThreadManager() :logger("ThreadManager") {};

    template<class _ThreadHandler>
    void addUserThread(const std::string& name, ThreadFunc func) {
        addThread<_ThreadHandler>(name, func, ThreadPriority::User);
    }
    void stopUserThread(const std::string& name) {
        checkUserThread(name);
        stopThread(name);
    }
    bool hasThread(const std::string& name) {
        return threads.find(name) != threads.end();
    }
    void checkUserThread(const std::string& name) {
        if (!hasThread(name)) throw std::invalid_argument("Thread (named " + name + " doesn't exist");
        if (threads[name].priority == ThreadPriority::System) {
            throw std::runtime_error("Access denied.");
        }
    }
    void Stop() {
        for (auto& th : threads) {
            th.second.handler->Stop();
        }
        threads.clear();
    }
protected:
    template<class _ThreadHandler>
    void addThread(const std::string& name, ThreadFunc func, ThreadPriority priority) {
        std::lock_guard<std::mutex> lock(mtx);
        if (!hasThread(name)) {
            threads.insert(std::make_pair(name, ThreadObject(func, priority, std::make_shared<_ThreadHandler>())));

            std::thread([this, name]() { /// undefined
                auto& object = threads[name];
                object.handler->Start();

                this->runThread(name, object);
            }).detach();
        }
    }
    // ֹͣ�߳�
    void stopThread(const std::string& name) {
        std::lock_guard<std::mutex> lock(mtx);
        if (hasThread(name)) {
            threads[name].handler->Stop();
            threads.erase(name);
        }
    }

    // ���һ��ϵͳ�̣߳�ϵͳ�̲߳��ܱ�ɾ��
    template<class _ThreadHandler>
    void addSystemThread(const std::string& name, ThreadFunc func) {
        addThread<_ThreadHandler>(name, func, ThreadPriority::System);
    }
    // �����̵߳ĺ���
    void runThread(const std::string& name, ThreadObject& object) {
        try {

            object.func(object.handler);

            //auto future = std::async(std::launch::async, object.func, object.handler);
            //if (future.wait_for(object.handler->getTimeoutDuration()) == std::future_status::timeout) {
            //    logger.error("Thread ", name, " timed out after ", object.handler->getTimeoutDuration().count(), "ms.");
            //}
        }
        catch (const std::exception& e) {
            logger.error("Thread ", name, " encountered an error: ", e.what());
        }
        object.handler->Stop();
    }
};
