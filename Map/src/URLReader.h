#pragma once
#include <string>
#include <sstream>
#include <fstream>
//https://api.mapbox.com/styles/v1/mapbox/streets-v11/tiles/512/0/0/0?access_token=pk.eyJ1IjoibWFwYm94IiwiYSI6ImNpejY4NXFhYTA2bTMyeW44ZG0ybXBkMHkifQ.gUGbDOPUN1v1fTs5SeOR4A
#define CONFIG "Config.ini"

struct URL
{
	std::string url;
	std::string token;
	int resol;
	int x;
	int y;
};

class URLReader
{
public:
	URLReader();
	~URLReader() {};
	std::string MakeURL();
	void SetParams(int resol, int x, int y);
	void SetSite(const std::string& url);
	void SetToken(const std::string& token);
	void ReadFile(const std::string& filepath);
	int Resol();
	int x();
	int y();
private:
	URL m_fullurl;
};
