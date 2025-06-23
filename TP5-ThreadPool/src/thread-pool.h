/**
 * File: thread-pool.h
 * -------------------
 * This class defines the ThreadPool class, which accepts a collection
 * of thunks (which are zero-argument functions that don't return a value)
 * and schedules them in a FIFO manner to be executed by a constant number
 * of child threads that exist solely to invoke previously scheduled thunks.
 */

#ifndef _thread_pool_
#define _thread_pool_

#include <cstddef>
#include <functional>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "Semaphore.h"

struct Worker {
    std::thread thread;
    std::function<void(void)> task;
    Semaphore workerSem{0};
    bool available = true;
    bool taskReady = false;
};


class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    void schedule(const std::function<void(void)>& thunk);
    void wait();
    ~ThreadPool();

private:

    void dispatcher();          
    void worker(size_t id);     

    std::vector<Worker> threadWorkers;  // Lista de workers del pool
    std::queue<std::function<void(void)>> pendingTasks;  // Cola de tareas pendientes
    std::mutex pendingTasksMutex;  // Protege acceso a pendingTasks

    std::mutex workerStateMutex;  // Protege acceso a estado de los workers (task, available, taskReady)

    bool isShuttingDown = false;  // Indica si el pool está cerrándose
    size_t tasksInProgress = 0;   // Cantidad de tareas activas (en ejecución o por ejecutar)
    std::mutex progressMutex;     // Protege tasksInProgress
    std::condition_variable allTasksDoneCV;  // Notifica cuando tasksInProgress llega a 0

    std::thread taskAssignerThread;   // Hilo que reparte tareas a los workers
    Semaphore queuedTasks{0};         // Cuenta tareas en cola esperando ser asignadas
    Semaphore freeWorkers{0};         // Cuenta workers disponibles
    // Prohibir copiado para evitar problemas con threads y semáforos
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
};

#endif // _thread_pool_
