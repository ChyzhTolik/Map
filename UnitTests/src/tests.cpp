#include <catch2/catch_all.hpp>
#define CURL_STATICLIB
#include <curl/curl.h>
#include "URLReader.cpp"
#include "WebReader.h"
#include "WebReader.cpp"
#include <iostream>

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

TEST_CASE("Test tile creation", "[WEBReader]")
{
    WebReader reader;
    reader.MakeFile(1, 1, 1);
    REQUIRE(std::filesystem::exists("Tiles/tile111.png") == true);
    std::filesystem::remove("Tiles/tile111.png");
}

int main(int argc, char* argv[])
{
    // global setup...

    int result = Catch::Session().run(argc, argv);

    // global clean-up...
    return result;
}