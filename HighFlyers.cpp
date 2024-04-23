/// to jest moja zmiana dla https://github.com/daancode/a-star/
/// dodaje 3 wymiar a także próbuje zaimplementowac pare optymalizacji
/// są to optymalizacje czasowe, tym samym zajmują ZNACZNIE więcej pamięci
/// (głównie przez użycie wektora 3 wymiarowego jako przestrzeni z przeszkodami 
/// zamiast listy koordynatów)
/// zaimplementowane niektóre techniki z https://www.sciencedirect.com/science/article/abs/pii/S0957417423028154
/// jak również poprawia zwalnianie węzłów z openSet
/// 
/// stworzyłem przy pomocy chatgpt skrypt w pythonie który pozawala na zwizualizowanie ścieżki na grafie wolumetrycznym

#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include "MSSA.h"


int main()
{
    AStar::Generator generator;
    int size = 20;
    generator.setWorldSize({ size, size, size});
    std::string obstaclesFile = "przeszkody.txt";
    generator.load(obstaclesFile);
    generator.setHeuristic(AStar::Heuristic::euclidean);
    std::cout << "Generate path ... \n";
    auto path = generator.findPath({ 1, 1, 5 }, { 19, 19 ,5 });

    std::string pathFile = "trasa.txt";
    std::ofstream file(pathFile);
    for (auto& coordinate : path) {
        file << coordinate.x << "," << coordinate.y <<"," <<coordinate.z<< "\n";
    }
    file.close();
    std::string command = "py displayGraph.py --size "+std::to_string(size)+" "+std::to_string(size) + " "+std::to_string(size) + " " + obstaclesFile + " " + pathFile;
    int returnCode = system(command.c_str());
    if (returnCode != 0) {
        std::cerr << "Error executing Python script" << std::endl;
        return 1;
    }
    return 0;
}