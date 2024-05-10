#ifndef SETTINGS_CPP
#define SETTINGS_CPP

#include "settings.h"
#include "well_known.h"
#include "log.h"
#include <mutex> // Include the mutex header

// Define mutexes for thread synchronization
std::mutex settingsMutex;
std::mutex logMutex;

int setting_virus_ctrl_virus_found_action = 0;
std::string setting_server_server_url = "nan";
std::string excluded_folders[1000];
int excluded_folders_size = 0;
std::string included_folders[1000];
int included_folders_size = 0;
std::string disallowed_start_folders[1000];
int disallowed_start_folders_size = 0;
bool setting_rtp_folder_scan_status = true; // 0=off, 1=on
bool setting_rtp_process_scan_status = true; // 0=off, 1=on
bool setting_virus_ctrl_virus_process_found_kill = true; // 0=do not kill, 1=kill
bool setting_communication_unsafe_tls = false; // 0=do not allow communication via unsafe, self-signed certs, 1=allow communication via unsafe, self-signed certs
int srv_log_timeout = 0;
int log_timeout_reset = 0;

void load_included_folders();
void load_excluded_folders();
void load_disallowed_start_folders();

int load_settings() {
    //std::lock_guard<std::mutex> lock(settingsMutex); // Lock access to settings file operations

    std::ifstream file(SETTINGS_DB);

    if (!file.is_open()) {
        log(LOGLEVEL::ERR, "[load_settings()]: Could not open settings file. ", SETTINGS_DB);
        setting_server_server_url = "nan";
        return 1;
    }

    std::string settings_cmd, settings_arg;
    file>>settings_cmd;//the firs tline is our cert
    while (file >> settings_cmd) {
        file >> settings_arg;

        // Process settings
        //printf("aaaaaaaa,%s::,%s\n",settings_cmd.c_str(), settings_arg.c_str());
        if (settings_cmd == "virus_ctrl:virus_found:action") {
            if (settings_arg == "remove") {
                setting_virus_ctrl_virus_found_action = 1;
            }
            else if (settings_arg == "quarantine") {
                setting_virus_ctrl_virus_found_action = 2;
            }
            else if (settings_arg == "ignore") {
                setting_virus_ctrl_virus_found_action = 3;
            }
            else if (settings_arg == "call_srv") {
                setting_virus_ctrl_virus_found_action = 4;
            }
        }
        else if (settings_cmd == "server:server_url") {
            setting_server_server_url = settings_arg;
        }
        else if (settings_cmd == "rtp_folder_scan:status") {
            setting_rtp_folder_scan_status = (settings_arg == "true");
        }
        else if (settings_cmd == "rtp_process_scan:status") {
            setting_rtp_process_scan_status = (settings_arg == "true");
        }
        else if (settings_cmd == "communication:unsafe_tls") {
            setting_communication_unsafe_tls = (settings_arg == "allow");
        }
        else if (settings_cmd == "virus_ctrl:virus_process_found:kill") {
            setting_virus_ctrl_virus_process_found_kill = (settings_arg == "true");
        }
    }

    file.close();

    load_included_folders();
    load_excluded_folders();
    load_disallowed_start_folders();

    return 0;
}

// We have two different get_setting functions. One for int and one for std::string return values
int get_setting(const std::string& setting_name) {
    std::lock_guard<std::mutex> lock(settingsMutex); // Lock access to settings variables

    if (setting_name == "virus_ctrl:virus_found:action") {
        return setting_virus_ctrl_virus_found_action;
    }
    if (setting_name == "virus_ctrl:virus_process_found:kill") {
        return setting_virus_ctrl_virus_process_found_kill;
    }
    else if (setting_name == "rtp_folder_scan:status") {
        return setting_rtp_folder_scan_status;
    }
    else if (setting_name == "rtp_process_scan:status") {
        return setting_rtp_process_scan_status;
    }
    else if (setting_name == "communication:unsafe_tls") {
        return setting_communication_unsafe_tls;
    }

    return -1;
}

std::string get_setting_string(const std::string& setting_name) {
    std::lock_guard<std::mutex> lock(settingsMutex); // Lock access to settings variables

    if (setting_name == "server:server_url") {
        return setting_server_server_url;
    }

    return "nan";
}

void load_included_folders() {
    std::lock_guard<std::mutex> lock(settingsMutex); // Lock access to settings variables

    std::ifstream file(INCLUDED_FOLDERS);

    if (!file.is_open()) {
        log(LOGLEVEL::ERR, "[load_included_files()]: Could not open included folders file. ", INCLUDED_FOLDERS);
        return;
    }

    std::string line;
    std::getline(file, line); // Skip the first line
    while (std::getline(file, line)) {
        size_t start_pos = line.find('"'); // Find the position of the first double quote
        if (start_pos != std::string::npos) {
            size_t end_pos = line.find('"', start_pos + 1); // Find the position of the second double quote
            if (end_pos != std::string::npos) {
                std::string path = line.substr(start_pos + 1, end_pos - start_pos - 1); // Extract the path between double quotes
                included_folders[included_folders_size++] = path;
            }
        }
    }

    file.close();
}

void load_excluded_folders() {
    std::lock_guard<std::mutex> lock(settingsMutex); // Lock access to settings variables

    std::ifstream file(EXCLUDED_FOLDERS);

    if (!file.is_open()) {
        log(LOGLEVEL::ERR, "[load_included_files()]: Could not open excluded folders file. ", INCLUDED_FOLDERS);
        return;
    }

    std::string line;
    std::getline(file, line); // Skip the first line
    while (std::getline(file, line)) {
        size_t start_pos = line.find('"'); // Find the position of the first double quote
        if (start_pos != std::string::npos) {
            size_t end_pos = line.find('"', start_pos + 1); // Find the position of the second double quote
            if (end_pos != std::string::npos) {
                std::string path = line.substr(start_pos + 1, end_pos - start_pos - 1); // Extract the path between double quotes
                excluded_folders[excluded_folders_size++] = path;
            }
        }
    }

    file.close();
}

void load_disallowed_start_folders() {
    std::lock_guard<std::mutex> lock(settingsMutex); // Lock access to settings variables
	std::ifstream file(DISALLOWED_START_FOLDERS);
    if (!file.is_open()) {
        printf("Could not open disallowed start folders file. %s\n", DISALLOWED_START_FOLDERS);
		log(LOGLEVEL::ERR, "[load_disallowed_start_folders()]: Could not open disallowed start folders file. ", DISALLOWED_START_FOLDERS);
		return;
	}

	std::string line;
	std::getline(file, line); // Skip the first line
    while (std::getline(file, line)) {
		size_t start_pos = line.find('"'); // Find the position of the first double quote
        if (start_pos != std::string::npos) {
			size_t end_pos = line.find('"', start_pos + 1); // Find the position of the second double quote
            if (end_pos != std::string::npos) {
				std::string path = line.substr(start_pos + 1, end_pos - start_pos - 1); // Extract the path between double quotes
				disallowed_start_folders[disallowed_start_folders_size++] = path;
			}
		}
	}

	file.close();
}

bool is_folder_included(const std::string& path) {
    std::lock_guard<std::mutex> lock(settingsMutex); // Lock access to settings variables

    for (int i = 0; i < included_folders_size; i++) {
        if (path.find(included_folders[i]) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool is_folder_excluded(const std::string& path) {
    std::lock_guard<std::mutex> lock(settingsMutex); // Lock access to settings variables

    for (int i = 0; i < excluded_folders_size; i++) {
        if (path.find(excluded_folders[i]) != std::string::npos) {
            return true;
        }
    }
    return false;
}
bool is_disallowed_sart_folder(const std::string& path) {
	std::lock_guard<std::mutex> lock(settingsMutex); // Lock access to settings variables

    for (int i = 0; i < disallowed_start_folders_size; i++) {
        if (path.find(disallowed_start_folders[i]) != std::string::npos) {
			return true;
		}
	}
	return false;
}



void print_inclusions() {
    std::lock_guard<std::mutex> lock(logMutex); // Lock access to log printing

    for (int i = 0; i < excluded_folders_size; i++) {
        log(LOGLEVEL::INFO, "[print_exclusions()]: Excluded folder: ", excluded_folders[i]);
    }
}

int log_timeout_get() {
    std::lock_guard<std::mutex> lock(settingsMutex); // Lock access to settings variables

    return srv_log_timeout;
}

void log_timeout_set(int timeout) {
    std::lock_guard<std::mutex> lock(settingsMutex); // Lock access to settings variables

    srv_log_timeout = timeout;
}

void log_timeout_reset_set(int timeout) {
    std::lock_guard<std::mutex> lock(settingsMutex); // Lock access to settings variables

    log_timeout_reset = timeout;
}

int log_timeout_reset_get() {
    std::lock_guard<std::mutex> lock(settingsMutex); // Lock access to settings variables

    return log_timeout_reset;
}

#endif // SETTINGS_CPP
