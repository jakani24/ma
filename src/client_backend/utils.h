#pragma once
#include <string>
#include <iostream>
#include <windows.h>
std::string get_filename(const std::string& path);
int strcasecmp(const std::string& s1, const std::string& s2);
void kill_process(const std::string& path);
void startup(LPCTSTR lpApplicationName);
void split(const std::string& input, char delimiter, std::string& out1, std::string& out2);