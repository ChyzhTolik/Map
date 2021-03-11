
#include "WebReader.h"
#include "Map.h"
#include <filesystem>
#include <iostream>
int main(void)
{
    Map::Run(); 
    std::cout << "Delete tiles? y/n" << std::endl;
    char answer;
    std::cin >> answer;
    if (answer=='y')
    {
        for (const auto& entry : std::filesystem::directory_iterator("Tiles/"))
            std::filesystem::remove_all(entry.path());
    }
    return 0;
}

