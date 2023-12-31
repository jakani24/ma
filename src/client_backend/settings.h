#pragma once
#ifndef SETTINGS_H
#define SETTINGS_H
#include <iostream>
#include <Windows.h>
int get_setting(const char* setting_name);
int get_setting(const char* setting_name,char*out);
int load_settings();
bool is_folder_excluded(const char* path);
void print_exclusions();
#endif