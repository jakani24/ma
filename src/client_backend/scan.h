#pragma once
#include <string>
#include <unordered_map>
#include <Windows.h>
#include <string>
#include <fstream>
#include <iostream>
#include <cctype> // for std::isxdigit
#include <iostream>
#include <future>
#include <vector>
#include <algorithm>
void scan_folder_recursive(const std::string& directory);
void action_scanfile(const char* filepath);
//void action_scanfile_t(const char* filepath);
void action_scanfile_t(const std::string& filepath_);
void initialize(const std::string& folderPath);
void cleanup();