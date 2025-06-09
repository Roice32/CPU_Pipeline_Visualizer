#pragma once

#include <queue>
#include <mutex>
#include <iostream>

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
    if (container.empty())
    {
      std::cerr << "Attempt to pop from an empty queue" << std::endl;
      exit(EXIT_FAILURE);
    }

    T data = container.front();
    container.pop();
    return data;
  }

  T peek()
  {
    std::lock_guard<std::mutex> lock(guard);
    if (container.empty())
    {
      std::cerr << "Attempt to peek from an empty queue" << std::endl;
      exit(EXIT_FAILURE);
    }

    T data = container.front();
    return data;
  }
};