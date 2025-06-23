#include "thread-pool.h"
#include <stdexcept>

ThreadPool::ThreadPool(size_t numThreads)
    : workers(numThreads), shuttingDown(false), activeTasks(0)
{
    // Inicializar cada worker y lanzar su hilo
    for (size_t i = 0; i < numThreads; ++i) {
        workers[i].taskReady = false;   // No hay tarea asignada aún
        workers[i].available = true;    // Worker libre inicialmente
        
        // Lanzar hilo worker que esperará tareas asignadas
        workers[i].thread = std::thread(&ThreadPool::worker, this, i);
        workersAvailable.signal();      // Señalamos que este worker está disponible
    }

    // Lanzar hilo dispatcher encargado de asignar tareas a workers libres
    dispatcherThread = std::thread(&ThreadPool::dispatcher, this);
}

void ThreadPool::schedule(const std::function<void(void)>& task) {
    if (!task) throw std::invalid_argument("Tarea vacía no permitida");

    {
        std::lock_guard<std::mutex> lock(taskQueueMutex);
        if (shuttingDown) 
            throw std::runtime_error("No se aceptan tareas, pool cerrándose");
        taskQueue.push(task);  
    }

    {
        std::lock_guard<std::mutex> lock(activeTasksMutex);
        ++activeTasks;         // Incrementar contador de tareas activas
    }

    tasksInQueue.signal();     // Avisar al dispatcher que hay una tarea pendiente
}

void ThreadPool::dispatcher() {
    while (true) {
        tasksInQueue.wait();   // Esperar hasta que haya alguna tarea pendiente

        std::function<void()> task;

        {
            std::lock_guard<std::mutex> lock(taskQueueMutex);
            // Si se está cerrando y no quedan tareas, salir
            if (shuttingDown && taskQueue.empty()) break;

            // Obtener la siguiente tarea
            if (!taskQueue.empty()) {
                task = taskQueue.front();
                taskQueue.pop();
            } else {
                continue;   // Si no hay tareas, esperar siguiente señal
            }
        }

        workersAvailable.wait(); // Esperar que haya un worker libre

        // Buscar un worker disponible
        size_t id = workers.size();
        {
            for (size_t i = 0; i < workers.size(); ++i) {
                if (workers[i].available) {
                    id = i;
                    break;
                }
            }
        }

        if (id == workers.size()) {
            // No se encontró worker libre: volver a insertar tarea y seguir
            workersAvailable.signal();
            {
                std::lock_guard<std::mutex> lock(taskQueueMutex);
                taskQueue.push(task);
            }
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(workerMutex);
            // Asignar tarea al worker y marcarlo ocupado
            workers[id].task = task;
            workers[id].taskReady = true;
            workers[id].available = false;
        }

        workers[id].workerSem.signal(); // Despertar al worker para ejecutar la tarea
    }

    // Pool cerrándose: despertar a todos los workers para que finalicen
    for (auto& w : workers)
        w.workerSem.signal();
}

void ThreadPool::worker(size_t id) {
    while (true) {
        workers[id].workerSem.wait();  // Esperar señal de tarea o cierre

        std::function<void()> task;

        {
            std::lock_guard<std::mutex> lock(taskQueueMutex);
            // Si el pool está cerrando y no hay tarea asignada, terminar
            if (shuttingDown && !workers[id].taskReady)
                break;
        }

        {
            std::lock_guard<std::mutex> lock(workerMutex);
            // Tomar la tarea asignada y marcar worker como sin tarea
            if (workers[id].taskReady) {
                task = workers[id].task;
                workers[id].task = nullptr;
                workers[id].taskReady = false;
            }
        }

        if (task)
            task();  // Ejecutar la tarea

        {
            std::lock_guard<std::mutex> lock(workerMutex);
            workers[id].available = true;  // Marcar worker como libre
        }

        workersAvailable.signal();        // Indicar que hay un worker disponible

        {
            std::lock_guard<std::mutex> lock(activeTasksMutex);
            if (--activeTasks == 0)
                allDoneCv.notify_all();   // Notificar si ya no quedan tareas activas
        }
    }
}

void ThreadPool::wait() {
    // Bloquear hasta que todas las tareas hayan terminado
    std::unique_lock<std::mutex> lock(activeTasksMutex);
    allDoneCv.wait(lock, [this]() { return activeTasks == 0; });
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(taskQueueMutex);
        shuttingDown = true;   // Indicar cierre del pool
    }

    tasksInQueue.signal();     // Despertar al dispatcher para que termine

    if (dispatcherThread.joinable())
        dispatcherThread.join();

    // Despertar a todos los workers para que puedan finalizar
    for (auto& w : workers)
        w.workerSem.signal();

    // Esperar a que terminen los hilos workers
    for (auto& w : workers)
        if (w.thread.joinable())
            w.thread.join();
}
