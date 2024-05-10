/*
This file contains the functions to update the databases and settings of the program.

Functions:
	- update_yara: Updates the yara database.
	- update_db: Updates the signature database.
	- update_settings: Updates the settings.
	- action_update_settings: starts the update functions (settings).
	- action_update_db: starts the update functions (signatures and yara)
*/


#include <string.h>
#include <iostream>
#include "update.h"
#include "zip.h"
#include "setup_var.h"
#include "download.h"
#include "well_known.h"

int update_yara(const std::string& folder_path) {
    //remove the old databases
    char* url = new char[1000];
    strcpy_s(url, 1000, get_server("setup.txt"));
    strcat_s(url, 1000, "/database_srv/yara.zip");//need to add machine_id and apikey

    std::string output_path = folder_path + "\\" + "sig.zip";
    int res = download_file_from_srv(url, output_path.c_str());

    if (res != 0) {
        return 10; // Error downloading file
    }

    //unzip the file
    unzip(output_path, folder_path);
    return 0;
}

int update_db(const std::string& folder_path) {
    //remove the old databases
    char* url = new char[1000];
    strcpy_s(url, 1000, get_server("setup.txt"));
    strcat_s(url, 1000, "/database_srv/sig.zip");//need to add machine_id and apikey

    std::string output_path = folder_path + "\\" + "sig.zip";
    int res = download_file_from_srv(url, output_path.c_str());

    if (res != 0) {
        return 10; // Error downloading file
    }

    //unzip the file
    unzip(output_path, folder_path);
    return 0;
}
int update_settings(const char* settings_type) {
    //create the strings to download the files
    char* url = new char[1000];
    strcpy_s(url, 1000, get_server("setup.txt"));
    strcat_s(url, 1000, "/api/php/settings/get_settings.php?");//need to add machine_id and apikey
    strcat_s(url, 1000, settings_type);
    strcat_s(url, 1000, "&machine_id=");
    strcat_s(url, 1000, get_machineid(SECRETS));
    strcat_s(url, 1000, "&apikey=");
    strcat_s(url, 1000, get_apikey(SECRETS));
    int res = 1;
    if (strcmp(settings_type, "settings") == 0)
        res = download_file_from_srv(url, SETTINGS_DB);
    else if (strcmp(settings_type, "rtp_included") == 0)
        res = download_file_from_srv(url, INCLUDED_FOLDERS);
    else if (strcmp(settings_type, "rtp_excluded") == 0)
        res = download_file_from_srv(url, EXCLUDED_FOLDERS);
    else if (strcmp(settings_type, "sched") == 0)
        res = download_file_from_srv(url, SCHED_PATH);
    else if (strcmp(settings_type, "disallowed_start") == 0)
        res = download_file_from_srv(url, DISALLOWED_START_FOLDERS);
    //int res = 0;
    if (res != 0) {
        return 1;
    }

    delete[] url;
    return 0;
}
int action_update_settings() {
    //update the settings
    int err = 0;
    if (update_settings("settings") != 0) {
        err = 9;
    }
    //update the included folders
    if (update_settings("rtp_included") != 0) {
        err = 9;
    }
    //update the excluded folders
    if (update_settings("rtp_excluded") != 0) {
        err = 9;
    }
    //update the disallowed start folders
    if (update_settings("disallowed_start") != 0) {
        err = 9;
    }
    //update the schedule
    if (update_settings("sched") != 0) {
        err = 9;
    }
    return err;
}
int action_update_db() {
    //update the databases
    update_db(DB_DIR);
    return update_yara(YARA_DB_DIR);
}