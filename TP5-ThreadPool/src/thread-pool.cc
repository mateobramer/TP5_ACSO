#include "thread-pool.h"
#include <stdexcept>

ThreadPool::ThreadPool(size_t numThreads)
    : threadWorkers(numThreads), isShuttingDown(false), tasksInProgress(0)
{
    for (size_t i = 0; i < numThreads; ++i) {
        threadWorkers[i].taskReady = false;
        threadWorkers[i].available = true;

        threadWorkers[i].thread = std::thread(&ThreadPool::worker, this, i);
        freeWorkers.signal();
    }

    taskAssignerThread = std::thread(&ThreadPool::dispatcher, this);
}

void ThreadPool::schedule(const std::function<void(void)>& task) {
    if (!task) throw std::invalid_argument("Tarea vacía no permitida");

    {
        std::lock_guard<std::mutex> lock(pendingTasksMutex);
        if (isShuttingDown)
            throw std::runtime_error("No se aceptan tareas, pool cerrándose");
        pendingTasks.push(task);
    }

    {
        std::lock_guard<std::mutex> lock(progressMutex);
        ++tasksInProgress;
    }

    queuedTasks.signal();
}

void ThreadPool::dispatcher() {
    while (true) {
        queuedTasks.wait();

        std::function<void()> task;

        {
            std::lock_guard<std::mutex> lock(pendingTasksMutex);
            if (isShuttingDown && pendingTasks.empty()) break;

            if (!pendingTasks.empty()) {
                task = pendingTasks.front();
                pendingTasks.pop();
            } else {
                continue;
            }
        }

        freeWorkers.wait();

        size_t id = threadWorkers.size();
        {
            for (size_t i = 0; i < threadWorkers.size(); ++i) {
                if (threadWorkers[i].available) {
                    id = i;
                    break;
                }
            }
        }

        if (id == threadWorkers.size()) {
            freeWorkers.signal();
            {
                std::lock_guard<std::mutex> lock(pendingTasksMutex);
                pendingTasks.push(task);
            }
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(workerStateMutex);
            threadWorkers[id].task = task;
            threadWorkers[id].taskReady = true;
            threadWorkers[id].available = false;
        }

        threadWorkers[id].workerSem.signal();
    }

    for (auto& w : threadWorkers)
        w.workerSem.signal();
}

void ThreadPool::worker(size_t id) {
    while (true) {
        threadWorkers[id].workerSem.wait();

        std::function<void()> task;

        {
            std::lock_guard<std::mutex> lock(pendingTasksMutex);
            if (isShuttingDown && !threadWorkers[id].taskReady)
                break;
        }

        {
            std::lock_guard<std::mutex> lock(workerStateMutex);
            if (threadWorkers[id].taskReady) {
                task = threadWorkers[id].task;
                threadWorkers[id].task = nullptr;
                threadWorkers[id].taskReady = false;
            }
        }

        if (task)
            task();

        {
            std::lock_guard<std::mutex> lock(workerStateMutex);
            threadWorkers[id].available = true;
        }

        freeWorkers.signal();

        {
            std::lock_guard<std::mutex> lock(progressMutex);
            if (--tasksInProgress == 0)
                allTasksDoneCV.notify_all();
        }
    }
}

void ThreadPool::wait() {
    std::unique_lock<std::mutex> lock(progressMutex);
    allTasksDoneCV.wait(lock, [this]() { return tasksInProgress == 0; });
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(pendingTasksMutex);
        isShuttingDown = true;
    }

    queuedTasks.signal();

    if (taskAssignerThread.joinable())
        taskAssignerThread.join();

    for (auto& w : threadWorkers)
        w.workerSem.signal();

    for (auto& w : threadWorkers)
        if (w.thread.joinable())
            w.thread.join();
}
