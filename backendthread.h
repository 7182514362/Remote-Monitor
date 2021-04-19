#ifndef BACKENDTHREAD_H
#define BACKENDTHREAD_H

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>

class BackendThread
{
public:
    BackendThread();

    void addTask(const std::function<void()> &task);
    void stop() { m_stop = true; }

private:
    void run();

private:
    std::atomic<bool> m_stop;
    std::mutex m_mtx;               // for task queue
    std::condition_variable m_cond; // for task queue
    std::deque<std::function<void()>> m_taskQueue;

    std::thread m_thread;
};

#endif // BACKENDTHREAD_H
