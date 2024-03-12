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
char* setting_server_server_url = new char[300];
char excluded_folders[100][300];
int excluded_folders_size = 0;
char included_folders[100][300];
int included_folders_size = 0;
bool setting_rtp_folder_scan_status = 1; //0=off, 1=on
bool setting_rtp_process_scan_status = 1; //0=off, 1=on
bool setting_virus_ctrl_virus_process_found_kill = 1; //0=do not kill, 1=kill
bool setting_communication_unsafe_tls = 0; //0=do not allow communication via unsafe, self-signed certs, 1=allow communication via unsafe, self-signed certs
int srv_log_timeout = 0;
int log_timeout_reset = 0;

void load_included_folders();
void load_excluded_folders();

int load_settings() {
    FILE* fp;

    std::lock_guard<std::mutex> lock(settingsMutex); // Lock access to settings file operations

    if (fopen_s(&fp, SETTINGS_DB, "r") != 0) {
        log(LOGLEVEL::ERR, "[load_settings()]: Could not open settings file. ", SETTINGS_DB);
        strcpy_s(setting_server_server_url, 295, "nan");
        return 1;
    }
    else {
        char* settings_cmd = new char[505];
        char* settings_arg = new char[300];
        fscanf_s(fp, "%s", settings_cmd, 500); //the cert is always the first line
        while (!feof(fp)) {
            fscanf_s(fp, "%s", settings_cmd, 295); // get the command
            //now check which setting it is.
            if (strcmp(settings_cmd, "virus_ctrl:virus_found:action") == 0) {
                fscanf_s(fp, "%s", settings_arg, 295); // get the argument
                if (strcmp(settings_arg, "remove") == 0) {
                    setting_virus_ctrl_virus_found_action = 1; //1=remove
                }
                else if (strcmp(settings_arg, "quarantine") == 0) {
                    setting_virus_ctrl_virus_found_action = 2; //2=quarantine
                }
                else if (strcmp(settings_arg, "ignore") == 0) {
                    setting_virus_ctrl_virus_found_action = 3; //3=ignore
                }
                else if (strcmp(settings_arg, "call_srv") == 0) {
                    setting_virus_ctrl_virus_found_action = 4; //4=call_srv <= only call the server and tell it, do not remove or quarantine
                }
            }
            else if (strcmp(settings_cmd, "server:server_url") == 0) {
                fscanf_s(fp, "%s", settings_arg, 295); // get the argument
                strcpy_s(setting_server_server_url, 295, settings_arg);
            }
            else if (strcmp(settings_cmd, "rtp_folder_scan:status") == 0) {
                fscanf_s(fp, "%s", settings_arg, 295); // get the argument
                setting_rtp_folder_scan_status = (strcmp(settings_arg, "true") == 0); //1=on, 0=off
            }
            else if (strcmp(settings_cmd, "rtp_process_scan:status") == 0) {
                fscanf_s(fp, "%s", settings_arg, 295); // get the argument
                setting_rtp_process_scan_status = (strcmp(settings_arg, "true") == 0); //1=on, 0=off
            }
            else if (strcmp(settings_cmd, "communication:unsafe_tls") == 0) {
                fscanf_s(fp, "%s", settings_arg, 295); // get the argument
                setting_communication_unsafe_tls = (strcmp(settings_arg, "allow") == 0); //1=allow, 0=block
            }
            else if (strcmp(settings_cmd, "virus_ctrl:virus_process_found:kill") == 0) {
                fscanf_s(fp, "%s", settings_arg, 295); // get the argument
                setting_virus_ctrl_virus_process_found_kill = (strcmp(settings_arg, "true") == 0); //1=kill, 0=do not kill
            }
        }

        delete[] settings_cmd;
        delete[] settings_arg;
    }
    load_included_folders();
    load_excluded_folders();
    fclose(fp);
    return 0;
}

// We have two different get_setting functions. One for int and one for char* return values
int get_setting(const char* setting_name) {
    std::lock_guard<std::mutex> lock(settingsMutex); // Lock access to settings variables

    if (strcmp(setting_name, "virus_ctrl:virus_found:action") == 0) {
        return setting_virus_ctrl_virus_found_action;
    }
    if (strcmp(setting_name, "virus_ctrl:virus_process_found:kill") == 0) {
        return setting_virus_ctrl_virus_process_found_kill;
    }
    else if (strcmp(setting_name, "rtp_folder_scan:status") == 0) {
        return setting_rtp_folder_scan_status;
    }
    else if (strcmp(setting_name, "rtp_process_scan:status") == 0) {
        return setting_rtp_process_scan_status;
    }
    else if (strcmp(setting_name, "communication:unsafe_tls") == 0) {
        return setting_communication_unsafe_tls;
    }

    return -1;
}

int get_setting(const char* setting_name, char* out) {
    std::lock_guard<std::mutex> lock(settingsMutex); // Lock access to settings variables

    if (strcmp(setting_name, "server:server_url") == 0) {
        strcpy_s(out, 295, setting_server_server_url);
        return 0;
    }

    return -1;
}

void load_included_folders() {
    FILE* fp;
    if (fopen_s(&fp, INCLUDED_FOLDERS, "r") != 0) {
        log(LOGLEVEL::ERR, "[load_included_files()]: Could not open included folders file. ", INCLUDED_FOLDERS);
        return;
    }
    else {
        char* path = new char[505];
        fscanf_s(fp, "%s", path, 500); //the cert is always the first line
        while (!feof(fp)) {
            //get the path of an excluded folder
            path[0] = '\0';
            //the path is encapsulated with "
            int cnt = 0;
            int chr = 0;
            chr = fgetc(fp);
            if (chr == '\"') {
                chr = 0;
                while (cnt < 295 && chr != '\"') {
                    chr = fgetc(fp); //get a char
                    if (chr != '\"')
                        path[cnt] = chr;
                    path[cnt + 1] = '\0';
                    cnt++;
                }
                //now add the path to the array
                if (included_folders_size < 95) {
                    strcpy_s(included_folders[included_folders_size], 295, path);
                    included_folders_size++;
                }
                else {
                    log(LOGLEVEL::ERR, "[load_included_files()]: included folders array is full. Cannot add more folders.");
                }
            }
        }
        fclose(fp);
        delete[] path;
    }
}

void load_excluded_folders() {
    FILE* fp;
    if (fopen_s(&fp, EXCLUDED_FOLDERS, "r") != 0) {
        log(LOGLEVEL::ERR, "[load_excluded_folders()]: Could not open excluded folders file. ", EXCLUDED_FOLDERS);
        return;
    }
    else {
        char* path = new char[505];
        fscanf_s(fp, "%s", path, 500); //the cert is always the first line
        while (!feof(fp)) {
            //get the path of an excluded folder
            path[0] = '\0';
            //the path is encapsulated with "
            int cnt = 0;
            int chr = 0;
            chr = fgetc(fp);
            if (chr == '\"') {
                chr = 0;
                while (cnt < 295 && chr != '\"') {
                    chr = fgetc(fp); //get a char
                    if (chr != '\"')
                        path[cnt] = chr;
                    path[cnt + 1] = '\0';
                    cnt++;
                }
                //now add the path to the array
                if (excluded_folders_size < 95) {
                    strcpy_s(excluded_folders[excluded_folders_size], 295, path);
                    excluded_folders_size++;
                }
                else {
                    log(LOGLEVEL::ERR, "[load_excluded_folders()]: Excluded folders array is full. Cannot add more folders.");
                }
            }
        }
        fclose(fp);
        delete[] path;
    }
}

bool is_folder_included(const char* path) {
    std::lock_guard<std::mutex> lock(settingsMutex); // Lock access to settings variables

    for (int i = 0; i < included_folders_size; i++) {
        if (strstr(path, included_folders[i]) != 0 && strcmp(included_folders[i], "") != 0 && strcmp(included_folders[i], " ") != 0) {
            return true;
        }
    }
    return false;
}

bool is_folder_excluded(const char* path) {
    std::lock_guard<std::mutex> lock(settingsMutex); // Lock access to settings variables

    for (int i = 0; i < excluded_folders_size; i++) {
        if (strstr(path, excluded_folders[i]) != 0 && strcmp(excluded_folders[i], "") != 0 && strcmp(excluded_folders[i], " ") != 0) {
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

#endif
