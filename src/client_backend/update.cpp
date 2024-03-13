#ifndef UPDATE_CPP
#define UPDATE_CPP

#include "update.h"
#include "log.h"
#include "connect.h"
#include "settings.h"
#include "security.h"
#include <string>
#include <iostream>
#include <cctype>

int update_db(const std::string& folder_path) {
    // Download the databases from the server
    for (char firstChar = '0'; firstChar <= 'f'; ++firstChar) {
        for (char secondChar = '0'; secondChar <= 'f'; ++secondChar) {
            // Ensure that the characters are valid hexadecimal digits
            if (!std::isxdigit(firstChar) || !std::isxdigit(secondChar) || std::isupper(firstChar) || std::isupper(secondChar)) {
                continue;
            }

            // Create the filename based on the naming convention
            std::string file_name = std::string(1, firstChar) + secondChar + ".jdbf";

            // Create the strings to download the files
            std::string url = get_setting_string("server:server_url");
            if (url.empty() || url == "nan") {
                return 2; // Invalid server URL
            }
            url += "/database_srv/" + file_name;

            std::string output_path = folder_path + "\\" + file_name;

            std::cout << url << std::endl;

            int res = download_file_from_srv(url, output_path, get_setting("communication:unsafe_tls"));
            if (res != 0) {
                return 10; // Error downloading file
            }
        }
    }
    return 0;
}

int update_settings(const std::string& settings_type) {
    // Create the strings to download the files
    std::string url = get_setting_string("server:server_url");
    if (url.empty() || url == "nan") {
        return 2; // Invalid server URL
    }
    url += "/api/php/settings/get_settings.php?";
    url += settings_type + "&machine_id=" + get_machineid(SECRETS) + "&apikey=" + get_apikey(SECRETS);

    int res = 1;
    if (settings_type == "settings")
        res = download_file_from_srv(url, SETTINGS_DB, get_setting("communication:unsafe_tls"));
    else if (settings_type == "rtp_included")
        res = download_file_from_srv(url, INCLUDED_FOLDERS, get_setting("communication:unsafe_tls"));
    else if (settings_type == "rtp_excluded")
        res = download_file_from_srv(url, EXCLUDED_FOLDERS, get_setting("communication:unsafe_tls"));
    else if (settings_type == "sched")
        res = download_file_from_srv(url, SCHED_PATH, get_setting("communication:unsafe_tls"));

    if (res != 0) {
        log(LOGLEVEL::ERR, "[update_settings()]: Error downloading settings database file from server. ERROR:", res);
        return 1; // Error downloading file
    }

    return 0;
}

int action_update_settings() {
    // Update the settings
    int err = 0;
    if (update_settings("settings") != 0)
        err = 1;

    // Update the included folders
    if (update_settings("rtp_included") != 0)
        err = 2;

    // Update the excluded folders
    if (update_settings("rtp_excluded") != 0)
        err = 3;

    // Update the schedule
    if (update_settings("sched") != 0)
        err = 4;

    return err;
}

int action_update_db() {
    // Update the databases
    return update_db(DB_DIR);
}

#endif // UPDATE_CPP
