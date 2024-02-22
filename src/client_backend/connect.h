#pragma once
#include <iostream>
#include <iomanip>
#include <sstream>
#include <locale>
#include <string>
#include <curl/curl.h>
int connect_to_srv(const char* url, char* out, int max_len, bool ignore_insecure);
int download_file_from_srv(const char* url, const char* outputFileName,bool ignore_insecure);