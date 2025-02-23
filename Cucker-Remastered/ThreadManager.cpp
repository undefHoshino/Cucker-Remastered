#include "ThreadManager.h"

void ThreadManager::stopUserThread(const std::string& name) {
    checkUserThread(name);
    stopThread(name);
}

bool ThreadManager::hasThread(const std::string& name) {
    return threads.find(name) != threads.end();
}

void ThreadManager::checkUserThread(const std::string& name) {
    if (!hasThread(name)) throw std::invalid_argument("Thread (named " + name + " doesn't exist");
    if (threads[name].priority == ThreadPriority::System) {
        throw std::runtime_error("Access denied.");
    }
}

void ThreadManager::Stop() {
    std::vector<std::string> threadsToStop;

    for (const auto& th : threads) {
        threadsToStop.push_back(th.first);
    }

    for (const auto& name : threadsToStop) {
        stopThread(name);
    }

    threads.clear();
}


// 停止线程
void ThreadManager::stopThread(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    if (hasThread(name)) {
        auto& handler = threads[name].handler;
        handler.Stop();

        auto fut = handler.getFuture();
        auto status = fut.wait_for(std::chrono::seconds(1));

        if (status == std::future_status::timeout) {
            logger.error("Thread \"", name, "\" timed out after ", handler.getTimeoutDuration().count(), "ms.");
            throw ReportException("Thread \"", name, "\" timeout.");
        }
        else {
            int result = fut.get();
            logger.error("Thread \"", name, "\" exited with return code ", result);
        }
        threads.erase(name);
    }
}
// 运行线程的函数
void ThreadManager::runThread(const std::string& name, ThreadObject& object) {
    int _val = -1;
    try {
        _val = object.func(object.handler);
    }
    catch (const std::exception& e) {
        logger.error("Thread ", name, " encountered an error: ", e.what());
        _val = 0xccccccc;
    }
    object.handler.Return(_val);
}