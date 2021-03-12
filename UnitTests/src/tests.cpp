#include <catch2/catch_all.hpp>
#include "URLReader.cpp"


TEST_CASE("Test URL params", "[URLReader]") 
{
    URLReader url;
    url.SetParams(4, 5, 6);
    REQUIRE(url.Resol() == 4);
    REQUIRE(url.x() == 5);
    REQUIRE(url.y() == 6);
    url.SetSite("www.google.com/");
    url.SetToken("554");
    url.MakeURL();
    REQUIRE(url.MakeURL() == "www.google.com/4/5/6?access_token=554");
}

int main(int argc, char* argv[])
{
    // global setup...

    int result = Catch::Session().run(argc, argv);

    // global clean-up...

    return result;
}