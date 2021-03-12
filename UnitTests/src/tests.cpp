#include <catch2/catch_all.hpp>
#include "URLReader.h"

TEST_CASE("URLReader params", "[URLReader]") 
{
    URLReader reader;
    reader.SetParams(5, 6, 4);
    //REQUIRE(reader.x == 6);
    //REQUIRE(reader.y == 4);
    //REQUIRE(reader.Resol == 5);
}

int main(int argc, char* argv[])
{
    // global setup...

    int result = Catch::Session().run(argc, argv);

    // global clean-up...

    return result;
}