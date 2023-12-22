#pragma warning(disable:4996)
#ifndef CONNECT_CPP
#define CONNECT_CPP
#include "download.h"

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
        return 7;
    }

    // Set the URL to download
    curl_easy_setopt(curl, CURLOPT_URL, url);

    // Create a file to write the downloaded data
    output_file = fopen(outputFileName, "wb");
    if (!output_file) {
        curl_easy_cleanup(curl);
        return 7;
    }

    // Set the write callback function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, output_file);

    // Perform the download
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        return 7;
    }
    // Cleanup and close the file
    curl_easy_cleanup(curl);
    fclose(output_file);

    return 0;
}
#endif