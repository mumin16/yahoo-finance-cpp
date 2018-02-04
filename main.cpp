// main.cpp

#include <ctime>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <algorithm>


#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"libcurl.lib")
#define CURL_STATICLIB
#include <curl/curl.h>
#include <sstream>
static size_t write_stringstream(void *ptr, size_t size, size_t nmemb, void *stream) {
	std::string data((const char*)ptr, (size_t)size * nmemb);
	*((std::stringstream*)stream) << data;
	return size * nmemb;
}

int main()
{

	const char* stock = "GOOG";
	

	curl_global_init(CURL_GLOBAL_ALL);
	CURL *curl;
	CURLcode res;

	std::stringstream input;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);//10sec
		std::string crumb("https://finance.yahoo.com/quote/%5EDJI/history");
		curl_easy_setopt(curl, CURLOPT_URL, crumb);
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookie.txt");
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookie.txt");
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_stringstream);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &input);
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	if (res != CURLE_OK) {
		std::cout << "acces denied!" << std::endl;
		return 1;
	}
	else
	{
		time_t endDate = time(0);   // get time now (today)
		struct tm* tm = localtime(&endDate);
		tm->tm_year = tm->tm_year - 5;//5 ago from today
		time_t startDate = mktime(tm);
		char buffer[255];
		sprintf(buffer, "https://query1.finance.yahoo.com/v7/finance/download/%s?period1=%lld&period2=%lld&interval=1d&events=history&crumb=", stock, startDate, endDate);
		std::string url = buffer;

		std::string str = input.str();
		std::size_t pos = str.find("\"CrumbStore\":{\"crumb\":\"");     
		std::string str3 = str.substr(pos + 23, 11);    
		url.append(str3);
		input.str(std::string());//clear


		curl = curl_easy_init();
		if (curl) {

			curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);//10sec
			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookie.txt");
			curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookie.txt");
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_stringstream);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &input);
			curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		}
		if (res != CURLE_OK) {
			std::cout << "acces denied!" << std::endl;
			return 1;
		}

	}



	std::cout << input.str();
	return 0;
}