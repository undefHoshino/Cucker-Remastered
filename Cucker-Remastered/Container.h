#pragma once
#include <unordered_map>
#include <string>
#include <any>
#include <mutex>

class MapContainer {
private:
    std::unordered_map<std::string, std::any> container; 
    std::mutex mtx;
public:
    void setData(std::string key, std::any value) {
        std::lock_guard<std::mutex> lock(mtx); // 保证线程安全
        container[key] = value;
    }
    template <typename T>
    T& quote(std::string key) {
        if (!hasData(key)) {
            throw std::runtime_error("Data not found for key: " + key);
        }
        return std::any_cast<T&>(container[key]);
    }

    template <typename T>
    T getData(std::string key) {
        if (!hasData(key)) {
            throw std::runtime_error("Data not found for key: " + key);
        }

        T result = std::any_cast<T>(container[key]);
        return result;
    }

    // 获取数据并从容器中移除
    template <typename T>
    T getAndRemove(std::string key) {
        std::lock_guard<std::mutex> lock(mtx);
        T result = getData<T>(key);
        remove(key); // 删除数据
        return result;
    }

    template <typename T>
    T getOrDefault(std::string key, const T& defaultValue) {
        if (hasData(key)) {
            return getData<T>(key);
        }
        return defaultValue;
    }

    void putIfAbsent(std::string key, std::any value) {
        if (!hasData(key)) {
            setData(key, value);
        }
    }

    void putDefaults(const std::unordered_map<std::string, std::any>& defaults) {
        for (const auto& [key, value] : defaults) {
            putIfAbsent(key, value);
        }
    }

    void putBatches(const std::unordered_map<std::string, std::any>& defaults) {
        for (const auto& [key, value] : defaults) {
            setData(key, value);
        }
    }
    
    void remove(std::string key) {
        container.erase(key);
    }

    bool hasData(std::string key) {
        return container.count(key) > 0;
    }
};