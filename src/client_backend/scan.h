#pragma once
#include <string>
void ListFilesRecursive(const std::string& directory,int thread_id);
int scan_hash(const std::string& filename, const std::string& searchString);
int scan_hash(const char* hash);
int scan_hash(const char* hash);
void action_scanfile(const char* filepath);