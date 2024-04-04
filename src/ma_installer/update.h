#pragma once
#include <string>
int update_yara(const std::string& folder_path);
int update_db(const std::string& folder_path);
int update_settings(const char* settings_type);

int action_update_settings();
int action_update_db();
