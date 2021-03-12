#include "WebReader.h"

#include "URLReader.h"
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>

void WebReader::MakeFile(int resol, int x, int y)
{
	URLReader myurl;
	myurl.SetParams(resol, x, y);
	std::string furl = myurl.MakeURL();
	std::filesystem::create_directories("./Tiles");
	std::string tilename = "Tiles/tile";
	tilename += std::to_string(myurl.Resol()) + std::to_string(myurl.x()) + std::to_string(myurl.y()) + ".png";
	if (std::filesystem::exists(tilename))
	{
		return;
	}
	CURL* image;
	CURLcode imgresult;
	FILE* fp = nullptr;
	const char* url = furl.c_str();
	image = curl_easy_init();
	if (image)
	{
		// Open file 
		errno_t err;
		err = fopen_s(&fp, tilename.c_str(), "wb");
		if (fp == NULL)
			std::cout << "File cannot be opened";

		curl_easy_setopt(image, CURLOPT_WRITEFUNCTION, NULL);
		curl_easy_setopt(image, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(image, CURLOPT_URL, url);
		// Grab image 
		imgresult = curl_easy_perform(image);
		if (imgresult)
			std::cout << "Cannot grab the image!\n";
	}
	// Clean up the resources 
	curl_easy_cleanup(image);
	// Close the file 
	fclose(fp);
}