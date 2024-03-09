#pragma once
#ifndef SETTINGS_H
#define SETTINGS_H
#include <iostream>
#include <Windows.h>
int get_setting(const char* setting_name);
int get_setting(const char* setting_name,char*out);
int load_settings();
bool is_folder_included(const char* path);
bool is_folder_excluded(const char* path);
void print_inclusions();

int log_timeout_get();
void log_timeout_set(int timeout);
void log_timeout_reset_set(int timeout);
int log_timeout_reset_get();
#endif