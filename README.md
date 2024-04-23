Modyfikacja https://github.com/daancode/a-star/ dla 3 wymiaru
+ pare zmian algorytmicznych (lepsza wydajnosc czasowa, DUZO gorsza wydajnosc pamieciowa, na pewno do poprawy)


przykład użycia: 

```cpp
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include "source/MSSA.h"


int main()
{
    AStar::Generator generator;
    int size = 20;
    generator.setWorldSize({ size, size, size});

    std::string obstaclesFile = "przeszkody.txt";
    generator.load(obstaclesFile);

    generator.setHeuristic(AStar::Heuristic::manhattan);

    std::cout << "Generate path ... \n";
    auto path = generator.findPath({ 19, 6, 0 }, { 1, 19 ,5 });

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
```
wizualizacja sciezki na grafie za pomoca skryptu w pythnonie:
![image](https://github.com/ssasied/3D-A-/assets/115064635/99cea8d6-d38c-400c-b8a5-9600e542290e)
