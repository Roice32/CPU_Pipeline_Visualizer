#pragma once

#include <queue>
#include <mutex>

template <typename T>
class CommQueue
{
private:
    std::queue<T> container;
    std::mutex guard;

public:
    CommQueue() {};

    void push(T elem)
    {
        std::lock_guard<std::mutex> lock(guard);
        container.push(elem);
    }

    bool isEmpty()
    {
        std::lock_guard<std::mutex> lock(guard);
        return container.empty();
    }
    
    T pop()
    {
        std::lock_guard<std::mutex> lock(guard);
        T data = container.front();
        container.pop();
        return data;
    }
};