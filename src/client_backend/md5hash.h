#pragma warning(disable:4996)
#pragma once
#include <openssl/md5.h>
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <unordered_map>
int md5_file(const char* path, char* md5Hash);
//char* md5_file_t(const char* path_);
std::string md5_file_t(const std::string& path_);