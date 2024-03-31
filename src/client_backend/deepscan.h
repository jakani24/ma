#pragma once
#include <string>

void init_yara_rules(const char* folderPath);
bool deepscan_file_t(const std::string & filepath);
void deepscan_folder(const std::string& directory);
void action_deepscanfile(const std::string& filepath_);
void action_deepscanfolder(const std::string& folderpath);