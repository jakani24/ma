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
size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    FILE* file = (FILE*)userp;
    if (file) {
        fwrite(contents, 1, totalSize, file);
    }
    return totalSize;
}

int download_file_from_srv(const char* url, const char* outputFileName) {
    //use curl to download a file from a server

    CURL* curl;
    CURLcode res;
    FILE* output_file;

    curl = curl_easy_init();
    if (!curl) {
        return 1;
    }

    // Set the URL to download
    curl_easy_setopt(curl, CURLOPT_URL, url);

    // Create a file to write the downloaded data
    output_file = fopen(outputFileName, "wb");
    if (!output_file) {
        curl_easy_cleanup(curl);
        return 1;
    }

    // Set the write callback function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, output_file);

    // Perform the download
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        return 1;
    }
    // Cleanup and close the file
    curl_easy_cleanup(curl);
    fclose(output_file);

    return 0;
}
#endif