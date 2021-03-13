#pragma once
#define CURL_STATICLIB
#include <curl/curl.h>
#include <string>
#include <vector>
class WebReader
{
public:
	WebReader();
	~WebReader();
	void MakeFile(int resol,int x, int y);
	void MakeFiles(int resol, int x, int y);
	int download_asynchronous(void);
private:
	void add_transfer(CURLM* cm, int i);
	int resol, x, y;
	std::vector<std::string> urls;
};
