#pragma once
#ifndef UPDATE_H
#define UPDATE_H
#include <curl/curl.h>
#include<string>
int update_db(const std::string& folder_path);
int update_settings(const std::string& folder_path);
#endif