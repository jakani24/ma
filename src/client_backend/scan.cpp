#include "scan.h"
#include <windows.h>
#include <iostream>
#include <openssl/md5.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <time.h>
#include "md5hash.h"
#include <string>
#include "well_known.h"
#include "log.h"
#include "virus_ctrl.h"
#ifndef SCAN_CPP
#define SCAN_CPP
std::unordered_map<std::string, HANDLE> fileHandles;
std::unordered_map<std::string, HANDLE> mappingHandles;
std::unordered_map<std::string, char*> fileData;

int cnt = 0;
int num_threads=0;

//load all the db files into memory
void initialize(const std::string& folderPath) {
    for (char firstChar = '0'; firstChar <= 'f'; ++firstChar) {
        for (char secondChar = '0'; secondChar <= 'f'; ++secondChar) {
            // Ensure that the characters are valid hexadecimal digits
            if (!std::isxdigit(firstChar) || !std::isxdigit(secondChar) or std::isupper(firstChar) or std::isupper(secondChar)) {
                continue;
            }

            // Create the filename based on the naming convention
            std::string filename = folderPath +"\\" + firstChar + secondChar + ".jdbf";
            //printf("Loading %s\n", filename.c_str());

            // Open the file
            HANDLE hFile = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile == INVALID_HANDLE_VALUE) {
                log(LOGLEVEL::ERR, "[initialize()]: Error opening database file: ", filename);
                continue; // Move on to the next file if there's an error
            }

            // Create the file mapping
            HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
            if (hMapping == NULL) {
                log(LOGLEVEL::ERR, "[initialize()]: Error creating database file mapping: ", filename);
                CloseHandle(hFile);
                continue; // Move on to the next file if there's an error
            }

            // Map the file into memory
            char* fileDataPtr = static_cast<char*>(MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0));
            if (fileDataPtr == NULL) {
                log(LOGLEVEL::ERR, "[initialize()]: Error mapping database file: ", filename);
                CloseHandle(hMapping);
                CloseHandle(hFile);
                continue; // Move on to the next file if there's an error
            }

            // Store the handles in the global maps
            fileHandles[filename] = hFile;
            mappingHandles[filename] = hMapping;
            fileData[filename] = fileDataPtr;
        }
    }
}


// Call this function when you are done using the file mappings
void cleanup() {
    for (const auto& entry : fileHandles) {
        UnmapViewOfFile(fileData[entry.first]);
        CloseHandle(mappingHandles[entry.first]);
        CloseHandle(entry.second);
    }

    // Clear the global maps
    fileHandles.clear();
    mappingHandles.clear();
    fileData.clear();
}
//the latest and fastest version of searching a hash by now
int search_hash(const std::string& dbname_, const std::string& hash_, const std::string& filepath_) {
    // Check if the file mapping is already open for the given filename
    thread_local std::string dbname (dbname_);
    thread_local std::string hash (hash_);
    thread_local std::string filepath (filepath_);

    thread_local auto fileIter = fileHandles.find(dbname);
    if (fileIter == fileHandles.end()) {
        log(LOGLEVEL::ERR, "[search_hash()]: File mapping not initialized for ", dbname);
        return 2;
    }

    // Use fileData for subsequent searches
    thread_local DWORD fileSize = GetFileSize(fileHandles[dbname], NULL);
    thread_local std::string fileContent(fileData[dbname], fileSize);

    // Search for the specific string in the file content
    thread_local size_t foundPos = fileContent.find(hash);
    if (foundPos != std::string::npos) {
        log(LOGLEVEL::VIRUS, "[search_hash()]: Found virus: ", hash, " in file: ", filepath);
        virus_ctrl_store(filepath.c_str(), hash.c_str(), hash.c_str());
        //afterwards do the processing with that file
        virus_ctrl_process(hash.c_str());
        return 1; // Found
    }
    return 0; // Not found
}
bool file_exists(const std::string& filePath) {
    DWORD fileAttributes = GetFileAttributes(filePath.c_str());

    if (fileAttributes == INVALID_FILE_ATTRIBUTES) {
        // The file does not exist or there was an error
        return false;
    }

    // Check if it's a regular file and not a directory
    return (fileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

//this is the main function to scan folders. it will then start multuiple threads based on the number of cores / settings
void scan_folder_recursive(const std::string& directory) {
    std::string search_path = directory + "\\*.*";
    WIN32_FIND_DATA find_file_data;
    HANDLE hFind = FindFirstFile(search_path.c_str(), &find_file_data);

    if (hFind == INVALID_HANDLE_VALUE) {
        log(LOGLEVEL::ERR, "[ListFilesRecursive()]: Error opening directory: ", directory, " while scanning files inside folder.");
        return;
    }

    do {
        if (strcmp(find_file_data.cFileName, ".") == 0 || strcmp(find_file_data.cFileName, "..") == 0) {
            continue; // Skip the current and parent directories
        }


        const std::string full_path = directory + "\\" + find_file_data.cFileName;
        if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // If it's a directory, recurse into it
            scan_folder_recursive(full_path);
        }
        else {
           //action scanfile_t will start the trheads for scanning the hashes
            //action_scanfile_t(full_path.c_str());

            //do multithreading here
            while (num_threads >= std::thread::hardware_concurrency()) {
				Sleep(10);
			}
            num_threads++;
            std::thread scan_thread(action_scanfile_t, full_path);
            scan_thread.detach();

            cnt++;
            if (cnt % 100 == 0) {
                printf("Processed %d files;\n", cnt);
                //printf("Number of threads: %d\n", num_threads);
            }
        }
    } while (FindNextFile(hFind, &find_file_data) != 0);

    FindClose(hFind);
}



//for singlethreaded scans
void action_scanfile(const char*filepath) {
    char* db_path = new char[300];

    //log(LOGLEVEL::INFO, "[action_scanfile_t()]: Scanning file: ", filepath);
    if (strlen(filepath) == 0 or strcmp("", filepath) == 0 or file_exists(filepath) == false) {
        return; //no filepath given or file not accessible
    }
    else {
        char* hash = new char[300];
        hash[0] = '\0';
        strcpy_s(hash, 295, md5_file_t(filepath).c_str());
        sprintf_s(db_path, 295, "%s\\%c%c.jdbf", DB_DIR, hash[0], hash[1]);
        search_hash(db_path, hash, filepath);
        delete[] hash;
    }
    delete[] db_path;

}

/*
void action_scanfile_t( const char*filepath) {
    char* db_path = new char[300];
    int max_threads = std::thread::hardware_concurrency();
    //log(LOGLEVEL::INFO, "[action_scanfile_t()]: Scanning file: ", filepath);
    if (strlen(filepath) == 0 or strcmp("", filepath) == 0 or file_exists(filepath) == false) {
        return; //no filepath given or file not accessible
    }
    else {
        char* hash = new char[300];
        hash[0] = '\0';
        hash = md5_file_t(filepath);
        sprintf_s(db_path, 295, "%s\\%c%c.jdbf", DB_DIR, hash[0], hash[1]);
        while (num_threads >= max_threads) {
			Sleep(10);
		}
        num_threads++;
        std::thread search_thread(search_hash,db_path, hash, filepath);
        search_thread.detach();
        
        std::this_thread::sleep_for(std::chrono::microseconds(50));
        delete[] hash;
    }
    delete[] db_path;
}*/
void action_scanfile_t(const std::string& filepath_) {
    thread_local const std::string filepath (filepath_);
    thread_local char* db_path = new char[300];
    thread_local char*hash = new char[300];
    strcpy_s(hash,295 ,md5_file_t(filepath).c_str());
    sprintf_s(db_path, 295, "%s\\%c%c.jdbf", DB_DIR, hash[0], hash[1]);
    search_hash(db_path, hash, filepath);
    num_threads--;
}
#endif