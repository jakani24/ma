/*
This is the deepscan.cpp file. This file contains the implementation of the deepscan functions

Functions:
- load_yara_rules(): This function loads the YARA rules from a file and compiles them.
- init_yara_rules(): This function initializes the YARA rules by loading them from a folder.
- deepscan_folder(): This function scans all the files in a folder recursively.
- deepscan_file_t(): This function scans a single file using YARA rules.
- action_deepscanfolder(): This function is the action function for deepscanfolder.
- action_deepscanfile(): This function is the action function for deepscanfile.


*/

#include "deepscan.h"
#include "virus_ctrl.h"
#include "md5hash.h"
#include "well_known.h"
#include "log.h"
#include "utils.h"
#include "scan.h"
#include "app_ctrl.h"
#include <thread>
#include <iostream>
#include <yara.h>
#include <string>
#include <vector>
#include <stack>
#include <filesystem>
#include <mutex>

std::vector<YR_RULES*> compiled_rules; //global variable to store the compiled rules
std::mutex yara_scan_mutex;
int deep_cnt = 0;
int deep_all_files = 0;
int action_deepscan_is_virus = 0; //flag that is set by the callback function to tell the action_deepscan function if the file is a virus or not. this is needed to talk to the desktop client


// Load YARA rules from a file and compile them
YR_RULES* load_yara_rules(const char* ruleFilePath, YR_RULES* compiledRules = nullptr) {
    // Create a new compiler
    YR_COMPILER* compiler;
    if (yr_compiler_create(&compiler) != ERROR_SUCCESS) {
		log(LOGLEVEL::ERR_NOSEND, "[load_yara_rules()]: Failed to create YARA compiler.");
        return nullptr;
    }

    FILE* file;
    fopen_s(&file, ruleFilePath, "r");
    if (file == nullptr) {
        yr_compiler_destroy(compiler);
        return nullptr;
    }
    int result = yr_compiler_add_file(compiler, file, nullptr, ruleFilePath);
    if (result != ERROR_SUCCESS) {
        //std::cerr << "Failed to compile YARA rules from file: " << ruleFilePath << std::endl;
        log(LOGLEVEL::ERR_NOSEND, "[load_yara_rules()]: Failed to compile YARA rules from file: ", ruleFilePath);
        yr_compiler_destroy(compiler);
        fclose(file);
        return nullptr;
    }

    // Get rules from compiler and add them to the compiledRules object
    yr_compiler_get_rules(compiler, &compiledRules);

    // Destroy the compiler
    yr_compiler_destroy(compiler);
    fclose(file);
    return compiledRules;
}

// Initialize YARA rules by loading them from a folder
void init_yara_rules(const char* folderPath) {

    // Stack to store directories to be traversed iteratively
    std::stack<std::string> directories;
    directories.push(folderPath);

    while (!directories.empty()) {
        std::string currentDir = directories.top();
        directories.pop();

        for (const auto& entry : std::filesystem::directory_iterator(currentDir)) {
            if (entry.is_regular_file()) {
                std::string filePath = entry.path().string();
                if (filePath.ends_with(".yar") || filePath.ends_with(".yara")) {
                    YR_RULES* rules = load_yara_rules(filePath.c_str());
                    if (rules != nullptr) {
                        compiled_rules.push_back(rules);
                    }
                }
            }
            else if (entry.is_directory()) {
                directories.push(entry.path().string());
            }
        }
    }
    set_yara_ready();
}


std::stack<std::string> deep_directories; // Stack to store directories to be scanned


// Scan all files in a folder recursively using first the normal scanner, then the deep scanner
void deepscan_folder(const std::string& directory) {
    if (is_yara_ready()) {
        deep_directories.push(directory);

        while (!deep_directories.empty()) {
            std::string current_dir = deep_directories.top();
            deep_directories.pop();

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
                        deep_directories.push(full_path);
                    }
                    else {
                        if (debug_mode())
                            log(LOGLEVEL::INFO_NOSEND, "[deepscan_folder()]: Scanning file: ", full_path);

                        // Do multithreading here
                        int thread_timeout = 0;
                        //log(LOGLEVEL::INFO_NOSEND, "[scan_folder()]: Scanning file: ", full_path);
                        while (get_num_threads() >= std::thread::hardware_concurrency() && thread_safety()) {
                            Sleep(10);
                            thread_timeout++;
                            //printf("Thread timeout: %d\n", thread_timeout);
                            if (thread_timeout == 100 * 20) {
                                // If there is no available thread for more than 30 seconds, reset the thread counter
                                set_num_threads(0);
                            }
                        }
                        //log(LOGLEVEL::INFO_NOSEND, "[scan_folder()]: Scanning file: ", full_path);
                        if (is_valid_path(full_path)) { // Filter out invalid paths and paths with weird characters
                            std::uintmax_t fileSize = 0;
                            try {
                                fileSize = std::filesystem::file_size(full_path);
                            }
                            catch (std::filesystem::filesystem_error& e) {
                                log(LOGLEVEL::ERR_NOSEND, "[deepscan_folder()]: Could not get file size for file: ", full_path);
                            }

                            //std::uintmax_t fileSize = std::filesystem::file_size(full_path);
                            if (fileSize > 4000000000) { // 4GB
                                log(LOGLEVEL::INFO_NOSEND, "[deepscan_folder()]: File too large to scan: ", full_path);
                            }
                            else {
                                std::thread scan_thread(deepscan_file_t, full_path);
                                scan_thread.detach();
                            }
                        }
                        else {
                            log(LOGLEVEL::INFO_NOSEND, "[deepscan_folder()]: Invalid path: ", full_path);
                        }
                        deep_cnt++;
                        if (deep_cnt % 100 == 0) {
                            printf("Processed %d files;\n", deep_cnt);
                            //printf("Number of threads: %d\n", num_threads);
                        }
                        if (deep_cnt % 100 == 0) {
                            int actual_threads = get_num_running_threads();
                            if (get_num_threads() > actual_threads)
                                set_num_threads(actual_threads);//correct value of threads
                            printf("Number of threads: %d\n", get_num_threads());
                            //send progress to com file
                            std::ofstream answer_com(ANSWER_COM_PATH, std::ios::app);
                            if (answer_com.is_open()) {
                                answer_com << "progress " << (deep_cnt * 100 / (deep_all_files + 1)) << "\n";
                                answer_com.close();
                            }
                        }
                    }
                } while (FindNextFile(hFind, &find_file_data) != 0);
                FindClose(hFind);
            }
            else {
                log(LOGLEVEL::ERR_NOSEND, "[deepscan_folder()]: Could not open directory: ", current_dir, " while scanning files inside directory.");
            }
        }
    }

}


struct Callback_data {
    std::string filepath;
    // You can add more data members here if needed
};

// Callback function for YARA scan
int process_callback(YR_SCAN_CONTEXT* context,int message, void* message_data, void* user_data) {
    switch (message) {
        case CALLBACK_MSG_RULE_MATCHING:
        {
            // Access filepath from CallbackData
            Callback_data* callback_data = (Callback_data*)user_data;

            // Access filepath from CallbackData
            std::string filepath = callback_data->filepath;
            //we calculate the hash of the file so the virus ctrl functions are able to process it
            std::string hash = md5_file_t(filepath);

            virus_ctrl_store(filepath, hash, hash);
            //afterwards do the processing with that file
            virus_ctrl_process(hash);
            action_deepscan_is_virus = 1;
            break;
        }
    }
	return CALLBACK_CONTINUE;
}

// Callback function for YARA process scan scan
int process_callback_for_process(YR_SCAN_CONTEXT* context, int message, void* message_data, void* user_data) {
    switch (message) {
    case CALLBACK_MSG_RULE_MATCHING:
    {
        // Access filepath from CallbackData
        Callback_data* callback_data = (Callback_data*)user_data;

        // Access filepath from CallbackData
        std::string filepath = callback_data->filepath;
        //we calculate the hash of the file so the virus ctrl functions are able to process it
        std::string hash = md5_file_t(filepath);

        if (get_setting("virus_ctrl:virus_process_found:kill") == 1) {
            //kill the process
            kill_process(filepath.c_str());
            log(LOGLEVEL::VIRUS, "[deepscan_process_t()]: Killing process: ", filepath);
        }

        virus_ctrl_store(filepath, hash, hash);
        //afterwards do the processing with that file
        virus_ctrl_process(hash);
        action_deepscan_is_virus = 1;
        break;
    }
    }
    return CALLBACK_CONTINUE;
}

// Scan a single file using YARA rules (thread-safe)
bool deepscan_file_t(const std::string& file_path) {
    set_num_threads(get_num_threads() + 1);
    if (is_yara_ready()) {
        // we do not need to make a new instance of yara rules, because they are global and do not get deleted or modified
        thread_local std::string file_path_(file_path);
        // first we scan the file with the normal scanner, which means md5
        thread_local std::string hash(md5_file_t(file_path));
        thread_local char* db_path = new char[300];

        sprintf_s(db_path, 295, "%s\\%c%c.jdbf", DB_DIR, hash[0], hash[1]);
        if (search_hash(db_path, hash, file_path) != 1) { // if we already found a match in the database, we do not need to scan the file with yara
            // Load file into memory
            std::ifstream file_stream(file_path, std::ios::binary | std::ios::ate);
            if (!file_stream.is_open()) {
                // handle error if file cannot be opened
                set_num_threads(get_num_threads() - 1);
                return false;
            }
            std::streamsize file_size = file_stream.tellg();
            file_stream.seekg(0, std::ios::beg);
            std::vector<char> file_content(file_size);
            if (!file_stream.read(file_content.data(), file_size)) {
                // handle error if file content cannot be read
                set_num_threads(get_num_threads() - 1);
                return false;
            }
            file_stream.close();

            // get globally set yara rules and iterate over them
            Callback_data* callback_data = new Callback_data();
            for (YR_RULES* rule : compiled_rules) {
                callback_data->filepath = file_path_;
                yr_rules_scan_mem(rule, reinterpret_cast<const uint8_t*>(file_content.data()), file_content.size(), 0, process_callback, callback_data, 5000);
            }
        }
    }

    set_num_threads(get_num_threads() - 1);
    return true;
}

bool deepscan_process_t(const std::string& filepath_) {
    set_num_threads(get_num_threads() + 1);
    if (is_yara_ready()) {
        thread_local const std::string filepath(filepath_);
        std::ifstream file_stream(filepath, std::ios::binary | std::ios::ate);
        if (!file_stream.is_open()) {
            // handle error if file cannot be opened
            set_num_threads(get_num_threads() - 1);
            return false;
        }
        std::streamsize file_size = file_stream.tellg();
        file_stream.seekg(0, std::ios::beg);
        std::vector<char> file_content(file_size);
        if (!file_stream.read(file_content.data(), file_size)) {
            // handle error if file content cannot be read
            set_num_threads(get_num_threads() - 1);
            return false;
        }
        file_stream.close();

        // get globally set yara rules and iterate over them
        Callback_data* callback_data = new Callback_data();
        for (YR_RULES* rule : compiled_rules) {
            callback_data->filepath = filepath_;
            yr_rules_scan_mem(rule, reinterpret_cast<const uint8_t*>(file_content.data()), file_content.size(), 0, process_callback_for_process, callback_data, 5000);
        }
    }

    set_num_threads(get_num_threads() - 1);
}

// Action function for deepscanfolder
void action_deepscanfolder(const std::string& folderpath) {
    thread_init();
    if (is_yara_ready()) {
        thread_local std::string folderpath_(folderpath);
        deep_cnt = 0;
        deep_all_files = get_num_files(folderpath_);
        //tell the desktop client that the scan has started
        std::ofstream answer_com1(ANSWER_COM_PATH, std::ios::app);
        if (answer_com1.is_open()) {
            answer_com1 << "start " << deep_all_files << "\n";
            answer_com1.close();
        }
        deepscan_folder(folderpath_);
        std::ofstream answer_com(ANSWER_COM_PATH, std::ios::app);
        if (answer_com.is_open()) {
            answer_com << "end " << "\"" << "nothing" << "\"" << " " << "nothing" << " " << "nothing" << "\n";
            answer_com.close();
        }
    }
    thread_shutdown();
}

//for singlethreaded scans
void action_deepscanfile(const std::string& filepath_) {
    thread_init();
    if (is_yara_ready()) {
        std::string file_path(filepath_);
        char* db_path = new char[300];
        action_deepscan_is_virus = 0;
        //printf("start\n");
        if (is_valid_path(file_path)) { //filter out invalid paths and paths with weird characters
            //first scan the file with the normal scanner, which means md5
            thread_local char* db_path = new char[300];
            thread_local std::string hash(md5_file_t(file_path));
            sprintf_s(db_path, 295, "%s\\%c%c.jdbf", DB_DIR, hash[0], hash[1]);
            if (search_hash(db_path, hash, file_path) != 1) { //if we allready found a match in the database, we do not need to scan the file with yara
                deepscan_file_t(file_path);
                if (action_deepscan_is_virus == 0) {
                    std::ofstream answer_com(ANSWER_COM_PATH, std::ios::app);
                    if (answer_com.is_open()) {
                        answer_com << "not_found " << "\"" << file_path << "\"" << " " << hash << " " << "no_action_taken" << "\n";
                        answer_com.close();
                    }
                }
            }
        }
        else
            log(LOGLEVEL::INFO_NOSEND, "[action_scanfile()]: Invalid path: ", file_path);
    }
    thread_shutdown();
}