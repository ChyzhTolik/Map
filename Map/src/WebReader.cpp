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

size_t write_to_file(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t realsize = size * nmemb;
	auto file = reinterpret_cast<std::ofstream*>(userp);
	file->write(reinterpret_cast<const char*>(contents), realsize);
	return realsize;
} 

void save_to_file(CURL* curl,int i)
{
	std::string fname = "downloaded_data"+std::to_string(i)+".png";
	static std::ofstream file(fname, std::ios::binary);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_file);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, reinterpret_cast<void*>(&file));
	/*FILE* fp = nullptr;
	fopen_s(&fp, fname.c_str(), "wb");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	fclose(fp);*/
}

void set_ssl(CURL* curl)
{
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
} 

WebReader::WebReader() :resol(1), x(0), y(0)
{
	//curl_global_init(CURL_GLOBAL_DEFAULT);
}

WebReader::~WebReader()
{
	//curl_global_cleanup();
}

void WebReader::add_transfer(CURLM* cm, int i)
{
	CURL* eh = curl_easy_init();
	curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, NULL);
	curl_easy_setopt(eh, CURLOPT_URL, urls[i]);
	curl_easy_setopt(eh, CURLOPT_PRIVATE, urls[i]);
	curl_multi_add_handle(cm, eh);
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

void WebReader::MakeFiles(int resol, int x, int y)
{
	URLReader myurl;
	myurl.SetParams(resol, x, y);
	std::string furl;
	furl = myurl.MakeURL();
	urls.push_back(furl);
	myurl.SetParams(resol, x+1, y);
	furl = myurl.MakeURL();
	urls.push_back(furl);
	myurl.SetParams(resol, x, y+1);
	furl = myurl.MakeURL();
	urls.push_back(furl);
	myurl.SetParams(resol, x+1, y+1);
	furl = myurl.MakeURL();
	urls.push_back(furl);
	///
	std::vector<CURL*> handles(4);
	for (size_t i = 0; i < 4; i++)
	{
		handles[i]= curl_easy_init();
	}
	for (size_t i=0; i < handles.size();i++)
	{
		/* set options */
		//handles[i] = curl_easy_init();
		curl_easy_setopt(handles[i], CURLOPT_URL, "https://api.mapbox.com/styles/v1/mapbox/streets-v11/tiles/512/0/0/0?access_token=pk.eyJ1IjoibWFwYm94IiwiYSI6ImNpejY4NXFhYTA2bTMyeW44ZG0ybXBkMHkifQ.gUGbDOPUN1v1fTs5SeOR4A");
		set_ssl(handles[i]);
		save_to_file(handles[i],0);
		/* Perform the request, res will get the return code */
		auto res = curl_easy_perform(handles[i]);
		/* Check for errors */
		if (res != CURLE_OK)
		{
			std::cerr << "curl_easy_perform() failed:" <<
				curl_easy_strerror(res) << std::endl;
		}
		curl_easy_cleanup(handles[i]);
	}    
	//

	//CURLM* cm;
	//CURLMsg* msg;
	//unsigned int transfers = 0;
	//int msgs_left = -1;
	//int still_alive = 1;

	//curl_global_init(CURL_GLOBAL_ALL);
	//cm = curl_multi_init();

	///* Limit the amount of simultaneous connections curl should allow: */
	//curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, (long)MAX_PARALLEL);

	//for (transfers = 0; transfers < MAX_PARALLEL; transfers++)
	//	add_transfer(cm, transfers);

	//do {
	//	curl_multi_perform(cm, &still_alive);

	//	while ((msg = curl_multi_info_read(cm, &msgs_left))) {
	//		if (msg->msg == CURLMSG_DONE) {
	//			char* url;
	//			CURL* e = msg->easy_handle;
	//			curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &url);
	//			fprintf(stderr, "R: %d - %s <%s>\n",
	//				msg->data.result, curl_easy_strerror(msg->data.result), url);
	//			curl_multi_remove_handle(cm, e);
	//			curl_easy_cleanup(e);
	//		}
	//		else {
	//			fprintf(stderr, "E: CURLMsg (%d)\n", msg->msg);
	//		}
	//		if (transfers < urls.size())
	//			add_transfer(cm, transfers++);
	//	}
	//	if (still_alive)
	//		curl_multi_wait(cm, NULL, 0, 1000, NULL);

	//} while (still_alive || (transfers < urls.size()));

	//curl_multi_cleanup(cm);
	//curl_global_cleanup();
}

timeval get_timeout(CURLM* multi_handle)
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

int wait_if_needed(CURLM* multi_handle, timeval& timeout)
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

void multi_loop(CURLM* multi_handle)
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
int WebReader::download_asynchronous(void)
{
	std::vector<EasyHandle> handles(3);
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
	std::for_each(handles.begin(), handles.end(), [&i](auto& handle) 
		{
		curl_easy_setopt(handle.get(), CURLOPT_URL, "https://api.mapbox.com/styles/v1/mapbox/streets-v11/tiles/512/0/0/0?access_token=pk.eyJ1IjoibWFwYm94IiwiYSI6ImNpejY4NXFhYTA2bTMyeW44ZG0ybXBkMHkifQ.gUGbDOPUN1v1fTs5SeOR4A");
		curl_easy_setopt(handle.get(), CURLOPT_PRIVATE, "https://api.mapbox.com/styles/v1/mapbox/streets-v11/tiles/512/0/0/0?access_token=pk.eyJ1IjoibWFwYm94IiwiYSI6ImNpejY4NXFhYTA2bTMyeW44ZG0ybXBkMHkifQ.gUGbDOPUN1v1fTs5SeOR4A");
		set_ssl(handle.get());
		save_to_file(handle.get(),i);
		i++;
		});

	/* add the individual transfers */
	std::for_each(handles.begin(), handles.end(), [&multi_handle](auto& handle) {curl_multi_add_handle(multi_handle.get(), handle.get()); });

	multi_loop(multi_handle.get());

	std::for_each(handles.begin(), handles.end(), [&multi_handle](auto& handle) {curl_multi_remove_handle(multi_handle.get(), handle.get()); });
	return 0;
} 