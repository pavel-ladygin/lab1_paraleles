#include <iostream>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <chrono>

// Без синхронизации (для демонстрации проблем)
std::queue<int> noSynch;

// Функция для добавления задач в очередь (без синхронизации)
void add_noSynch(int id, int tasks) {
    for (int i = 0; i < tasks; ++i) {
        noSynch.push(id * 1000 + i);
    }
}

// Функция для извлечения задач из очереди (без синхронизации)
void ext_noSynch(int id) {
    while (!noSynch.empty()) {
        int task = noSynch.front(); 
        noSynch.pop(); 
    }
}
// Очередь задач с использованием atomic для синхронизации
std::queue<int> atomic_queue;
std::atomic<bool> done(false);

// Функция для добавления задач в очередь (с использованием atomic)
void add_atomic(int id, int tasks) {
    for (int i = 0; i < tasks; ++i) {
        atomic_queue.push(id * 1000 + i); 
    }
}
// Функция для извлечения задач из очереди (с использованием atomic)
void ext_atomic(int id) {
    while (!done || !atomic_queue.empty()) { 
        if (!atomic_queue.empty()) {
            int task = atomic_queue.front(); 
            atomic_queue.pop();
        }
    }
}
// Очередь задач с использованием mutex для синхронизации
std::queue<int> mutex_queue;
std::mutex mtx; 

// Функция для добавления задач в очередь (с использованием mutex)
void add_mutex(int id, int tasks) {
    for (int i = 0; i < tasks; ++i) {
        std::lock_guard<std::mutex> lock(mtx); 
        mutex_queue.push(id * 1000 + i);
    }
}
// Функция для извлечения задач из очереди (с использованием mutex)
void ext_mutex(int id) {
    while (true) {
        std::lock_guard<std::mutex> lock(mtx); 
        if (mutex_queue.empty()) break; 
        int task = mutex_queue.front(); 
        mutex_queue.pop();
    }
}
// Функция для тестирования всех подходов
void test_queue(int CntAdd, int CntExt, int task_add) {
    // Тестирование без синхронизации (демонстрация проблем)
    {
        std::vector<std::thread> Add, Ext;
        auto start = std::chrono::steady_clock::now();

        for (int i = 0; i < CntAdd; ++i) {
            Add.emplace_back(add_noSynch, i, task_add);
        }

        for (int i = 0; i < CntExt; ++i) {
            Ext.emplace_back(ext_noSynch, i);
        }

        for (auto& t : Add) t.join();
        for (auto& t : Ext) t.join();

        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "время без синхронизации: " << duration.count() << "мс" << std::endl;
    }

    // Тестирование с использованием atomic
    {
        std::vector<std::thread> Add, Ext;
        auto start = std::chrono::steady_clock::now();

        for (int i = 0; i < CntAdd; ++i) {
            Add.emplace_back(add_atomic, i, task_add);
        }

        for (int i = 0; i < CntExt; ++i) {
            Ext.emplace_back(ext_atomic, i);
        }

        for (auto& t : Add) t.join();

        done = true;

        for (auto& t : Ext) t.join();

        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "время Atomic: " << duration.count() << "мс" << std::endl;

        done = false;
    }

    // Тестирование с использованием mutex
    {
        std::vector<std::thread> Add, Ext;
        auto start = std::chrono::steady_clock::now();

        for (int i = 0; i < CntAdd; ++i) {
            Add.emplace_back(add_mutex, i, task_add);
        }

        for (int i = 0; i < CntExt; ++i) {
            Ext.emplace_back(ext_mutex, i);
        }

        for (auto& t : Add) t.join();
        for (auto& t : Ext) t.join();

        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "время Muteх: " << duration.count() << "мс" << std::endl;
    }
}

int main() {
    const int CntAdd = 2; 
    const int CntExt = 2;
    const int task_add = 1000; 

    test_queue(CntAdd, CntExt, task_add);

    return 0;
}
