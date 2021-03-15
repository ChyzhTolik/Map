#include "WebReader.h"

#include "URLReader.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <functional>
#include <thread>
#define MAX_PARALLEL 4

using EasyHandle = std::unique_ptr<CURL, std::function<void(CURL*)>>;
EasyHandle CreateEasyHandle()
{
	auto curl = EasyHandle(curl_easy_init(), curl_easy_cleanup);
	if (!curl)
	{
		throw std::runtime_error("Failed creating CURL easy object");
	}
	return curl;
} 

using MultiHandle = std::unique_ptr<CURLM, std::function<void(CURLM*)>>;
MultiHandle CreateMultiHandle()
{
	auto curl = MultiHandle(curl_multi_init(), curl_multi_cleanup);
	if (!curl)
	{
		throw std::runtime_error("Failed creating CURL multi object");
	}
	return curl;
}  

WebReader::WebReader() :resol(1), x(0), y(0)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

WebReader::~WebReader()
{
	curl_global_cleanup();
}

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
size_t WebReader::write_to_file(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t realsize = size * nmemb;
	auto file = reinterpret_cast<std::ofstream*>(userp);
	file->write(reinterpret_cast<const char*>(contents), realsize);
	return realsize;
}

void WebReader::save_to_file(CURL* curl, FILE* fp)
{
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
}

timeval WebReader::get_timeout(CURLM* multi_handle)
{
	long curl_timeo = -1;
	/* set a suitable timeout to play around with */
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	curl_multi_timeout(multi_handle, &curl_timeo);
	if (curl_timeo >= 0) {
		timeout.tv_sec = curl_timeo / 1000;
		if (timeout.tv_sec > 1)
			timeout.tv_sec = 1;
		else
			timeout.tv_usec = (curl_timeo % 1000) * 1000;
	}
	return timeout;
}

int WebReader::wait_if_needed(CURLM* multi_handle, timeval& timeout)
{
	fd_set fdread;
	fd_set fdwrite;
	fd_set fdexcep;
	FD_ZERO(&fdread);
	FD_ZERO(&fdwrite);
	FD_ZERO(&fdexcep);
	int maxfd = -1;
	/* get file descriptors from the transfers */
	auto mc = curl_multi_fdset(multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);

	if (mc != CURLM_OK) {
		std::cerr << "curl_multi_fdset() failed, code " << mc << "." << std::endl;
	}
	/* On success the value of maxfd is guaranteed to be >= -1. We call
		   sleep for 100ms, which is the minimum suggested value in the
		   curl_multi_fdset() doc. */
	if (maxfd == -1) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	int rc = maxfd != -1 ? select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout) : 0;
	return rc;
}

void WebReader::multi_loop(CURLM* multi_handle)
{
	int still_running = 0; /* keep number of running handles */

	/* we start some action by calling perform right away */
	curl_multi_perform(multi_handle, &still_running);

	while (still_running) {
		struct timeval timeout = get_timeout(multi_handle);

		auto rc = wait_if_needed(multi_handle, timeout);

		if (rc >= 0)
		{
			/* timeout or readable/writable sockets */
			curl_multi_perform(multi_handle, &still_running);
		}
		/* else select error */
	}
}
int WebReader::download_asynchronous(int resol, int x, int y)
{
	std::vector<URLReader> myurls(4);
	std::vector<FILE*> files(4);
	myurls[0].SetParams(resol, x, y);
	myurls[1].SetParams(resol, x+1, y);
	myurls[2].SetParams(resol, x, y+1);
	myurls[3].SetParams(resol, x+1, y+1);
	std::string furl; 
	std::filesystem::create_directories("./Tiles");
	std::string tilename;

	std::vector<EasyHandle> handles(4);
	MultiHandle multi_handle;

	/* init easy and multi stacks */
	try
	{
		multi_handle = CreateMultiHandle();
		std::for_each(handles.begin(), handles.end(), [](auto& handle) {handle = CreateEasyHandle(); });
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
		return -1;
	}
	/* set options */
	int i = 0;
	std::for_each(handles.begin(), handles.end(), [&i,this,&furl,&tilename,&myurls,&files](auto& handle) 
		{
			furl = myurls[i].MakeURL();
			std::string tilename = "Tiles/tile";
			tilename += std::to_string(myurls[i].Resol()) + std::to_string(myurls[i].x()) + std::to_string(myurls[i].y()) + ".png";
			fopen_s(&files[i], tilename.c_str(), "wb");
			curl_easy_setopt(handle.get(), CURLOPT_WRITEFUNCTION, NULL);
			curl_easy_setopt(handle.get(), CURLOPT_WRITEDATA, files[i]);
			//save_to_file(handle.get(), &files[i]);
			const char* url = furl.c_str();
			curl_easy_setopt(handle.get(), CURLOPT_URL, url);
			i++;
		});

	/* add the individual transfers */
	std::for_each(handles.begin(), handles.end(), [&multi_handle](auto& handle) {curl_multi_add_handle(multi_handle.get(), handle.get()); });

	multi_loop(multi_handle.get());

	std::for_each(handles.begin(), handles.end(), [&multi_handle](auto& handle) {curl_multi_remove_handle(multi_handle.get(), handle.get()); });
	for (size_t i = 0; i < files.size(); i++)
	{
		fclose(files[i]);
	}
	return 0;
} 