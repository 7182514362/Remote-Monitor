#include "backendthread.h"
#include <QDebug>

BackendThread::BackendThread()
    : m_stop(false)
    , m_mtx()
    , m_cond()
    , m_taskQueue()
    , m_thread(std::bind(&BackendThread::run, this))
{}

void BackendThread::addTask(const std::function<void()> &task)
{
    qDebug() << "BackendThread::addTask";
    std::lock_guard<std::mutex> lock(m_mtx);
    m_taskQueue.push_back(std::move(task));
    m_cond.notify_one();
}

void BackendThread::run()
{
    qDebug() << "BackendThread::run";

    while (!m_stop) {
        std::unique_lock<std::mutex> lock(m_mtx);
        while (m_taskQueue.empty()) {
            m_cond.wait(lock);
        }
        qDebug() << "taskQueue size: "<<m_taskQueue.size();
        for (auto &task : m_taskQueue) {
            task();
        }
        m_taskQueue.clear();
    }
}
