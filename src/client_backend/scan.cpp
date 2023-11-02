#include "scan.h"
#include <windows.h>
#include <iostream>
#include <openssl/md5.h>
#include <windows.h>
#include <iostream>
#include "md5hash.h"
#include <string>
#ifndef SCAN_CPP
#define SCAN_CPP
int cnt = 0;
void ListFilesRecursive(const std::string& directory, int thread_id) {
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
            ListFilesRecursive(full_path,thread_id);
        }
        else {
            // If it's a file, print its name
            /*char md5Hash[2 * MD5_DIGEST_LENGTH + 1];
            
            md5_file(full_path.c_str(), md5Hash);
            printf("%s\n", md5Hash);
            printf("%d\n", scan_hash("C:\\Users\\janis\\Documents\\ma_av_tests\\OutputFile.txt", "96be95b122c2b9b8bb5765c312ca4f73"));
            if (scan_hash("C:\\Users\\janis\\Documents\\ma_av_tests\\OutputFile.txt", md5Hash) == 1) {
                printf("virus found");

            }*/
            cnt++;
            if (cnt % 1000 == 0) {
                printf("Processed %d files; sent from thread %d\n", cnt,thread_id);
            }
        }
    } while (FindNextFile(hFind, &find_file_data) != 0);

    FindClose(hFind);
}
int scan_hash(const std::string& filename, const std::string& searchString) {//!!!! does not work with e.g. utf-16 or something like that. either ascii or utf8!!
    HANDLE hFile = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: Unable to open the file." << std::endl;
        return 2;
    }

    HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hMapping == NULL) {
        std::cerr << "Error: Unable to create file mapping." << std::endl;
        CloseHandle(hFile);
        return 2;
    }

    char* fileData = static_cast<char*>(MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0));
    if (fileData == NULL) {
        std::cerr << "Error: Unable to map the file into memory." << std::endl;
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return 2;
    }

    DWORD fileSize = GetFileSize(hFile, NULL);
    std::string fileContent(fileData, fileSize);

    // Search for the specific string in the file content
    size_t foundPos = fileContent.find(searchString);
    if (foundPos != std::string::npos) {
        UnmapViewOfFile(fileData);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return 1;//found
    }

    // Unmap the memory and close the handles
    UnmapViewOfFile(fileData);
    CloseHandle(hMapping);
    CloseHandle(hFile);
    return 0;
}
#endif