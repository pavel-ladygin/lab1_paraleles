#include <iostream>       
#include <vector>        
#include <algorithm>      
#include <chrono>         
#include <cstdlib>        
#include <boost/thread.hpp> 

// Функция, сортирующая по одному потоку
void SingleThreadSort(std::vector<int>& massiv) {
    std::sort(massiv.begin(), massiv.end()); 
}
// Функция, которая сортирует часть массива в потоке
void SortChast(std::vector<int>& section) {
    std::sort(section.begin(), section.end());
}
// Функция, сортирующая многопоточно
void MultiThreadSort(std::vector<int>& Massiv, int CntThreads) {
    const int MassivSize = Massiv.size();
    const int SectionSize = MassivSize / CntThreads; 
    std::vector<std::vector<int>> sections(CntThreads);


    for (int i = 0; i < CntThreads; ++i) {
        sections[i].resize(SectionSize);
        std::copy(Massiv.begin() + i * SectionSize, 
                  Massiv.begin() + (i + 1) * SectionSize,
                  sections[i].begin());
    }

    std::vector<boost::thread> threads;

    // Отдельные потоки для сортировки частей
    for (int i = 0; i < CntThreads; ++i) {
        threads.emplace_back(SortChast, std::ref(sections[i]));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // Соединяем все части
    std::vector<int> SortedMassiv;
    for (const auto& section : sections) {
        SortedMassiv.insert(SortedMassiv.end(), section.begin(), section.end());
    }

    // Сортируем полный массив
    std::sort(SortedMassiv.begin(), SortedMassiv.end());

    Massiv = SortedMassiv;
}

int main() {
    const int MassivSize = 1000000; 
    std::vector<int> Massiv(MassivSize);

    // Заполняем массив случайными числами
    std::srand(std::time(0)); 
    for (int i = 0; i < MassivSize; ++i) {
        Massiv[i] = std::rand() % 100000;
    }

    // Сортируем однопоточно
    std::vector<int> mas1 = Massiv; 
    auto start1 = std::chrono::steady_clock::now();
    SingleThreadSort(mas1); 
    auto end1 = std::chrono::steady_clock::now(); 
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);
    std::cout << "Однопоточная сортировка: " << duration1.count() << "мс" << std::endl; 

    // Сортируем многопоточно и тестируем на разном кол-ве потоков
    for (int CntThreads : {2, 4, 8}) { 
        std::vector<int> mas2 = Massiv; 
        auto start2 = std::chrono::steady_clock::now(); 
        MultiThreadSort(mas2, CntThreads); 
        auto end2 = std::chrono::steady_clock::now(); 
        auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2);
        std::cout << "Многопоточная сортировка с исп" << CntThreads << " потоков: " << duration2.count() << "мс" << std::endl; 
    }

    return 0;
}
