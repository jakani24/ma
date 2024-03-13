#pragma once
#include <iostream>
#include <iomanip>
#include <sstream>
#include <locale>
#include <string>
#include <curl/curl.h>
//int connect_to_srv(const char* url, char* out, int max_len, bool ignore_insecure);
//int download_file_from_srv(const char* url, const char* outputFileName,bool ignore_insecure);
//int fast_send(const char* url, bool ignore_insecure);
//char* url_encode(const char* input);
//int upload_to_srv(const char* url, const char* filepath, bool ignore_insecure);

int fast_send(const std::string& url, bool ignore_insecure);
int connect_to_srv(const std::string& url, char* out, int max_len, bool ignore_insecure);
int download_file_from_srv(const std::string& url, const std::string& outputFileName, bool ignore_insecure);
int upload_to_srv(const std::string& url, const std::string& filepath, bool ignore_insecure);
std::string url_encode(const std::string& input);