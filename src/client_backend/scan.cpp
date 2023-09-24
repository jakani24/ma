#include "scan.h"
#include <windows.h>
#include <iostream>

#include <windows.h>
#include <iostream>
#ifndef SCAN_CPP
#define SCAN_CPP
#include <string>
int cnt = 0;
void ListFilesRecursive(const std::string& directory) {
    std::string search_path = directory + "\\*.*";
    WIN32_FIND_DATA find_file_data;
    HANDLE hFind = FindFirstFile(search_path.c_str(), &find_file_data);

    if (hFind == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening directory: " << directory << std::endl;
        return;
    }

    do {
        if (strcmp(find_file_data.cFileName, ".") == 0 || strcmp(find_file_data.cFileName, "..") == 0) {
            continue; // Skip the current and parent directories
        }


        std::string full_path = directory + "\\" + find_file_data.cFileName;
        if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // If it's a directory, recurse into it
            ListFilesRecursive(full_path);
        }
        else {
            // If it's a file, print its name
            cnt++;
            if (cnt % 1000 == 0) {
                printf("Processed %d files\n", cnt);
            }
        }
    } while (FindNextFile(hFind, &find_file_data) != 0);

    FindClose(hFind);
}
#endif