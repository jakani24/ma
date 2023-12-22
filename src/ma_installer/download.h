#pragma once
#include <iostream>
#include <string>
#include <curl/curl.h>
int download_file_from_srv(const char* url, const char* outputFileName);
