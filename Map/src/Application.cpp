#include "Map.h"
#include <filesystem>
#include <iostream>
int main(void)
{
    Map::Run(); 
    
    for (const auto& entry : std::filesystem::directory_iterator("Tiles/"))
        std::filesystem::remove_all(entry.path());

    return 0;
} 

