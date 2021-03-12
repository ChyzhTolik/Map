#pragma once
#define CURL_STATICLIB
#include <curl/curl.h>

class WebReader
{
public:
	WebReader():resol(1), x(0), y(0){};
	~WebReader() {};
	void MakeFile(int resol,int x, int y);
private:
	int resol, x, y;
};
