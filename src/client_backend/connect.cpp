#pragma warning(disable:4996)
#ifndef CONNECT_CPP
#define CONNECT_CPP
#include "connect.h"

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int connect_to_srv(const char*url,char*out,int max_len, bool ignore_insecure) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        if(ignore_insecure==true)
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (max_len > (int)strlen(readBuffer.c_str())) {
            strcpy(out, readBuffer.c_str());
            return res;
        }
        else
            return  1; 
    }
    return 2;
}
#endif