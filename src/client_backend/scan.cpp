/*
This file contains the functions to scan files and folders for viruses.

Functions:
    - initialize(): This function loads all the database files into memory.
    - cleanup(): This function is called when you are done using the file mappings.
    - search_hash(): This function searches for a hash in a database file.
    - get_num_files(): This function gets the number of files in a directory and its subdirectories.
    - scan_folder(): This function scans a folder and its subdirectories for viruses.
    - action_scanfile(): This function scans a file for viruses. If called by scheduler
    - action_scanfolder(): This function scans a folder for viruses. If called by scheduler
    - scan_file_t(): This function scans a file for viruses in a separate thread.
    - scan_process_t(): This function scans a process for viruses in a separate thread.
    - do_quickscan(): This function performs a quick scan of the system.

*/


#ifndef SCAN_CPP
#define SCAN_CPP
#include "scan.h"
#include <windows.h>
#include <iostream>
#include <openssl/md5.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <stack>
#include <time.h>
#include "md5hash.h"
#include <string>
#include "well_known.h"
#include "log.h"
#include "virus_ctrl.h"
#include "app_ctrl.h"
#include <mutex> // Include the mutex header
#include <filesystem>
#include "utils.h"

// Define mutexes for thread synchronization
std::mutex fileHandlesMutex;
std::mutex mappingHandlesMutex;
std::mutex fileDataMutex;
std::mutex cntMutex;
std::mutex sizeHandlesMutex;

std::unordered_map<std::string, HANDLE> fileHandles;
std::unordered_map<std::string, HANDLE> mappingHandles;
std::unordered_map<std::string, DWORD> sizeHandles;
std::unordered_map<std::string, char*> fileData;

int cnt = 0;

int all_files = 0;


//load all the db files into memory
int initialize(const std::string& folderPath) {
    for (char firstChar = '0'; firstChar <= 'f'; ++firstChar) {
        for (char secondChar = '0'; secondChar <= 'f'; ++secondChar) {
            // Ensure that the characters are valid hexadecimal digits
            if (!std::isxdigit(firstChar) || !std::isxdigit(secondChar) or std::isupper(firstChar) or std::isupper(secondChar)) {
                continue;
            }

            // Create the filename based on the naming convention
            std::string filename = folderPath + "\\" + firstChar + secondChar + ".jdbf";
            //printf("Loading %s\n", filename.c_str());

            // Open the file
            HANDLE hFile = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile == INVALID_HANDLE_VALUE) {
                //log(LOGLEVEL::ERR, "[initialize()]: Error opening database file: ", filename);
                return 1;
            }

            // Create the file mapping
            HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
            if (hMapping == NULL) {
                // log(LOGLEVEL::ERR, "[initialize()]: Error creating database file mapping: ", filename);
                CloseHandle(hFile);
                return 2;
            }

            // Map the file into memory
            char* fileDataPtr = static_cast<char*>(MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0));
            if (fileDataPtr == NULL) {
                //log(LOGLEVEL::ERR, "[initialize()]: Error mapping database file: ", filename);
                CloseHandle(hMapping);
                CloseHandle(hFile);
                return 3;
            }

            // Store the handles in the global maps
            {
                std::lock_guard<std::mutex> lock(fileHandlesMutex);
                fileHandles[filename] = hFile;
            }
            {
                std::lock_guard<std::mutex> lock(mappingHandlesMutex);
                mappingHandles[filename] = hMapping;
            }
            {
                std::lock_guard<std::mutex> lock(fileDataMutex);
                fileData[filename] = fileDataPtr;
            }
            {
                std::lock_guard<std::mutex> lock(sizeHandlesMutex);
				sizeHandles[filename] = GetFileSize(hFile, NULL);
            }
            CloseHandle(hFile);
        }
    }
    return 0;
}

// Call this function when you are done using the file mappings
void cleanup() {
    for (const auto& entry : fileHandles) {
        UnmapViewOfFile(fileData[entry.first]);
        CloseHandle(mappingHandles[entry.first]);
        CloseHandle(entry.second);
    }

    // Clear the global maps
    {
        std::lock_guard<std::mutex> lock(fileHandlesMutex);
        fileHandles.clear();
    }
    {
        std::lock_guard<std::mutex> lock(mappingHandlesMutex);
        mappingHandles.clear();
    }
    {
        std::lock_guard<std::mutex> lock(fileDataMutex);
        fileData.clear();
    }
}

//the latest and fastest version of searching a hash by now
int search_hash(const std::string& dbname_, const std::string& hash_, const std::string& filepath_) {
    // Check if the file mapping is already open for the given filename
    thread_local std::string dbname;
    thread_local std::string hash;
    thread_local std::string filepath;
    {
        std::lock_guard<std::mutex> lock(fileHandlesMutex);
        dbname = dbname_;
    }
    {
        std::lock_guard<std::mutex> lock(fileDataMutex);
        hash = hash_;
    }
    {
        std::lock_guard<std::mutex> lock(mappingHandlesMutex);
        filepath = filepath_;
    }


    // Use fileData for subsequent searches
    DWORD fileSize;
    std::string fileContent;
    {
        std::lock_guard<std::mutex> lock(fileDataMutex);
        fileSize = sizeHandles[dbname];
        fileContent = std::string(fileData[dbname], fileSize);
    }

    // Search for the specific string in the file content
    size_t foundPos = fileContent.find(hash);
    if (foundPos != std::string::npos) {
        //log(LOGLEVEL::VIRUS, "[search_hash()]: Found virus: ", hash, " in file: ", filepath);
        virus_ctrl_store(filepath, hash, hash);
        //afterwards do the processing with that file
        virus_ctrl_process(hash);
        return 1; // Found
    }
    return 0; // Not found
}

//function to get num of files in idr and its subdirs etc (iterative)
int get_num_files(const std::string& directory) {
    std::string search_path = directory + "\\*.*";
    WIN32_FIND_DATA find_file_data;
    HANDLE hFind = FindFirstFile(search_path.c_str(), &find_file_data);
    int num_files = 0;

    if (hFind == INVALID_HANDLE_VALUE) {
        //std::cerr << "[get_num_files()]: Could not open directory: " << search_path << " while scanning files inside directory." << std::endl;
        log(LOGLEVEL::ERR_NOSEND, "[get_num_files()]: Could not open directory: ", search_path, " while scanning files inside directory.");
        return 0;
    }

    // Stack to store directories to be traversed iteratively
    std::stack<std::string> directories;
    directories.push(directory);

    while (!directories.empty()) {
        std::string current_dir = directories.top();
        directories.pop();

        search_path = current_dir + "\\*.*";
        hFind = FindFirstFile(search_path.c_str(), &find_file_data);

        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (strcmp(find_file_data.cFileName, ".") == 0 || strcmp(find_file_data.cFileName, "..") == 0) {
                    continue; // Skip the current and parent directories
                }

                const std::string full_path = current_dir + "\\" + find_file_data.cFileName;
                if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    // If it's a directory, add it to the stack
                    directories.push(full_path);
                }
                else {
                    num_files++;
                }
            } while (FindNextFile(hFind, &find_file_data) != 0);
            FindClose(hFind);
        }
        else {
            log(LOGLEVEL::ERR_NOSEND, "[get_num_files()]: Could not open directory: ", current_dir, " while scanning files inside directory.");
            //std::cerr << "[get_num_files()]: Could not open directory: " << current_dir << " while scanning files inside directory." << std::endl;
        }
    }

    return num_files;
}

//this is the main function to scan folders. it will then start multuiple threads based on the number of cores / settings
std::stack<std::string> directories; // Stack to store directories to be scanned

void scan_folder(const std::string& directory) {
    directories.push(directory);

    while (!directories.empty()) {
        std::string current_dir = directories.top();
        directories.pop();

        std::string search_path = current_dir + "\\*.*";
        WIN32_FIND_DATA find_file_data;
        HANDLE hFind = FindFirstFile(search_path.c_str(), &find_file_data);

        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (strcmp(find_file_data.cFileName, ".") == 0 || strcmp(find_file_data.cFileName, "..") == 0) {
                    continue; // Skip the current and parent directories
                }

                const std::string full_path = current_dir + "\\" + find_file_data.cFileName;
                if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    // If it's a directory, add it to the stack
                    directories.push(full_path);
                }
                else {
                    if(debug_mode())
						log(LOGLEVEL::INFO_NOSEND, "[scan_folder()]: Scanning file: ", full_path);

                    int thread_timeout = 0;
                    //log(LOGLEVEL::INFO_NOSEND, "[scan_folder()]: Scanning file: ", full_path);
                    while (get_num_threads() >= std::thread::hardware_concurrency()) {
                        Sleep(10);
                        thread_timeout++;
                        if (thread_timeout == 100 * 20) {
                            // If there is no available thread for more than 20 seconds, reset the thread counter
                            set_num_threads(0);
                        }
                    }

                    std::thread scan_thread(scan_file_t, full_path);
                    scan_thread.detach();
                    
                    cnt++;
                    if (cnt % 100 == 0) {
                        printf("Processed %d files;\n", cnt);
                        //printf("Number of threads: %d\n", num_threads);
                    }
                    if (cnt % 1000 == 0) {
                        int actual_threads = get_num_running_threads();
                        if(get_num_threads()>actual_threads)
                            set_num_threads(actual_threads);//correct value of threads minus the main and the rtp thread
                        printf("Number of threads: %d\n", get_num_threads());
                        //send progress to com file
                        std::ofstream answer_com(ANSWER_COM_PATH, std::ios::app);
                        if (answer_com.is_open()) {
                            answer_com << "progress " << (cnt * 100 / (all_files + 1)) << "\n";
                            answer_com.close();
                        }
                    }
                }
            } while (FindNextFile(hFind, &find_file_data) != 0);
            FindClose(hFind);
        }
        else {
            log(LOGLEVEL::ERR_NOSEND, "[scan_folder()]: Could not open directory: ", current_dir, " while scanning files inside directory.");
        }
    }
}



//for singlethreaded scans
void action_scanfile(const std::string& filepath_) {
    thread_init();
    const std::string filepath(filepath_);
    char* db_path = new char[300];
    char* hash = new char[300];
    //printf("start\n");
    if (is_valid_path(filepath)) { //filter out invalid paths and paths with weird characters
        //printf("valid\n");
        std::string hash_(md5_file_t(filepath));
        if (strlen(hash_.c_str()) < 290)
            strcpy_s(hash, 295, hash_.c_str());
        else {
            strcpy_s(hash, 295, "");
            log(LOGLEVEL::ERR_NOSEND, "[scan_file_t()]: Could not calculate hash for file: ", filepath);
        }
        sprintf_s(db_path, 295, "%s\\%c%c.jdbf", DB_DIR, hash[0], hash[1]);
        if (search_hash(db_path, hash, filepath) != 1) {
            //notify desktop client by writing to answer_com file
            //if there is now virus, we notify here. if there is a virus we only notify in the virus_ctrl_process function
            std::ofstream answer_com(ANSWER_COM_PATH, std::ios::app);
            if (answer_com.is_open()) {
                answer_com << "not_found " << "\"" << filepath_ << "\"" << " " << hash << " " << "no_action_taken" << "\n";
                answer_com.close();
            }
        }
    }
	else
		log(LOGLEVEL::INFO_NOSEND, "[action_scanfile()]: Invalid path: ", filepath_);
    thread_shutdown();
}
void action_scanfolder(const std::string& folderpath) {
    thread_init();
    thread_local std::string folderpath_(folderpath);
    cnt = 0;
    all_files = get_num_files(folderpath_);
    //tell the desktop client that the scan has started
    std::ofstream answer_com1(ANSWER_COM_PATH, std::ios::app);
    if (answer_com1.is_open()) {
		answer_com1 << "start " << all_files << "\n";
		answer_com1.close();
	}
    scan_folder(folderpath_);
    std::ofstream answer_com(ANSWER_COM_PATH, std::ios::app);
    if (answer_com.is_open()) {
        answer_com << "end " << "\"" << "nothing" << "\"" << " " << "nothing" << " " << "nothing" << "\n";
        answer_com.close();
    }
    thread_shutdown();
}

void scan_file_t(const std::string& filepath_) {
    set_num_threads(get_num_threads() + 1);
    thread_local const std::string filepath(filepath_);
    thread_local char* db_path = new char[300];
	if (is_valid_path(filepath)) { //filter out invalid paths and paths with weird characters
        std::uintmax_t fileSize = 0;
        try {
            fileSize = std::filesystem::file_size(filepath);
		}
		catch (std::filesystem::filesystem_error& e) {
			log(LOGLEVEL::ERR_NOSEND, "[scan_file_t()]: Could not get file size for file: ", filepath);
            set_num_threads(get_num_threads() - 1);
			return;
		}
        if (fileSize > 4000000000) { // 4GB
            log(LOGLEVEL::INFO_NOSEND, "[scan_folder()]: File too large to scan: ", filepath);
        }
        else {
            thread_local std::string hash(md5_file_t(filepath));
            sprintf_s(db_path, 295, "%s\\%c%c.jdbf", DB_DIR, hash[0], hash[1]);
            search_hash(db_path, hash, filepath);
        }
    }
    else {
        log(LOGLEVEL::INFO_NOSEND, "[scan_folder()]: Invalid path: ", filepath);
    }
    set_num_threads(get_num_threads() - 1);
}
void scan_process_t(const std::string& filepath_) {
    set_num_threads(get_num_threads() + 1);
    thread_local const std::string filepath(filepath_);
    thread_local char* db_path = new char[300];
    thread_local char* hash = new char[300];
    strcpy_s(hash, 295, md5_file_t(filepath).c_str());
    sprintf_s(db_path, 295, "%s\\%c%c.jdbf", DB_DIR, hash[0], hash[1]);
    if (search_hash(db_path, hash, filepath) == 1) {
        //check if need to kill process
        if (get_setting("virus_ctrl:virus_process_found:kill") == 1) {
            //kill the process
            kill_process(filepath.c_str());
            log(LOGLEVEL::VIRUS, "[scan_process_t()]: Killing process: ", filepath);
        }
    }
    set_num_threads(get_num_threads() - 1);
}

void do_quickscan() {
    thread_init();
    //scan windows startup folders & commonly infected places
    //tell the desktop client that the scan has started
    std::ofstream answer_com1(ANSWER_COM_PATH, std::ios::app);
    if (answer_com1.is_open()) {
        answer_com1 << "start " << 0 << "\n";
        answer_com1.close();
    }
    //general startup folder
    scan_folder("C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\Startup");
	scan_folder("C:\\Users\\All Users\\Microsoft\\Windows\\Start Menu\\Programs\\Startup");
    
    //find every users startupfolder and scan it:
    std::string user_folder = "C:\\Users\\*";
    std::string user_folder_no_wildcrad = "C:\\Users";
    WIN32_FIND_DATA find_file_data;
    HANDLE hFind = FindFirstFile(user_folder.c_str(), &find_file_data);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(find_file_data.cFileName, ".") == 0 || strcmp(find_file_data.cFileName, "..") == 0) {
				continue; // Skip the current and parent directories
			}
			std::string full_path = user_folder_no_wildcrad + "\\" + find_file_data.cFileName + "\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup";
			scan_folder(full_path);
		} while (FindNextFile(hFind, &find_file_data) != 0);
		FindClose(hFind);
	}
    std::ofstream answer_com(ANSWER_COM_PATH, std::ios::app);
    if (answer_com.is_open()) {
        answer_com << "end " << "\"" << "nothing" << "\"" << " " << "nothing" << " " << "nothing" << "\n";
        answer_com.close();
    }
    thread_shutdown();
}
#endif