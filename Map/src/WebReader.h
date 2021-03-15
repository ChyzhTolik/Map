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
	int download_asynchronous(int resol, int x, int y);
private:
	void multi_loop(CURLM* multi_handle);
	timeval get_timeout(CURLM* multi_handle);
	int wait_if_needed(CURLM* multi_handle, timeval& timeout);
	size_t write_to_file(void* contents, size_t size, size_t nmemb, void* userp);
	int resol, x, y;
	std::vector<std::string> urls;
};
