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

class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    void schedule(const std::function<void(void)>& thunk);
    void wait();
    ~ThreadPool();

private:
    struct Worker {
        std::thread thread;               // Hilo que ejecuta las tareas
        std::function<void(void)> task;  // Tarea asignada actualmente
        Semaphore workerSem{0};           // Semáforo para señalizar tareas pendientes
        bool available = true;            // Indica si el worker está libre
        bool taskReady = false;           // Marca si hay tarea asignada para ejecutar
    };

    void dispatcher();          
    void worker(size_t id);     

    std::vector<Worker> workers;  
    std::queue<std::function<void(void)>> taskQueue;  
    std::mutex taskQueueMutex;                        // Mutex para proteger acceso a taskQueue

    std::mutex workerMutex;     // Mutex para proteger acceso a datos compartidos de workers (task, available, taskReady)

    bool shuttingDown = false;  
    size_t activeTasks = 0;    
    std::mutex activeTasksMutex; // Mutex para proteger activeTasks
    std::condition_variable allDoneCv; // Condición para notificar cuando activeTasks llega a 0

    std::thread dispatcherThread;   // Hilo dispatcher que gestiona asignación de tareas
    Semaphore tasksInQueue{0};       // Semáforo que cuenta tareas pendientes para despachar
    Semaphore workersAvailable{0};   // Semáforo que cuenta workers disponibles para ejecutar

    // Prohibir copiado para evitar problemas con threads y semáforos
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
};

#endif // _thread_pool_
