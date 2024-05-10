#pragma once
#ifndef SETTINGS_H
#define SETTINGS_H
#include <iostream>
#include <Windows.h>
#include <string>
int get_setting(const std::string& setting_name);
std::string get_setting_string(const std::string& setting_name);

int load_settings();
void load_included_folders();
void load_excluded_folders();
bool is_folder_included(const std::string& path);
bool is_folder_excluded(const std::string& path);
bool is_disallowed_sart_folder(const std::string& path);
void print_inclusions(); 

int log_timeout_get();
void log_timeout_set(int timeout);
void log_timeout_reset_set(int timeout);
int log_timeout_reset_get();
#endif