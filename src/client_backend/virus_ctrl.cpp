#ifndef VIRUS_CTRL_CPP
#define VIRUS_CTRL_CPP

#include "virus_ctrl.h"
#include "well_known.h"
#include "log.h"
#include "settings.h"
#include "connect.h"
#include "security.h"
#include "utils.h"
#include <mutex>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <thread>
#include <windows.h>
#include <tlhelp32.h>
#include <cstring>
#include <fstream>

// Define a mutex for thread synchronization
std::mutex virusCtrlMutex;

int virus_ctrl_store(const std::string& path, const std::string& hash, const std::string& id) {
    std::lock_guard<std::mutex> lock(virusCtrlMutex); // Lock the mutex
    std::ofstream file(VIRUS_CTRL_DB + id, std::ios::app);
    if (!file.is_open()) {
        log(LOGLEVEL::ERR, "[virus_ctrl_store()]: Failed to open virus control database to store path of infected file: ", path);
        return 1;
    }
    else {
        file << "\"" << path << "\" " << hash << "\n";
        file.close();
        return 0;
    }
}

int virus_ctrl_process(const std::string& id) {
    std::lock_guard<std::mutex> lock(virusCtrlMutex); // Lock the mutex
    // Take actions based on settings.
    // e.g., delete infected files, quarantine them, etc.
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait for the file to be written to the disk else the process that created the file might not be finished yet

    std::ifstream file(VIRUS_CTRL_DB + id);
    if (!file.is_open()) {
        log(LOGLEVEL::ERR, "[virus_ctrl_process()]: Failed to open virus control database to process it.", VIRUS_CTRL_DB + id);
        return 1;
    }
    else {
        std::string line;
        while (std::getline(file, line)) {
            // Get a full path (enclosed with "")
            std::string path, hash;
            std::size_t pos = line.find("\"");
            if (pos != std::string::npos) {
                path = line.substr(pos + 1, line.find("\"", pos + 1) - pos - 1);
                hash = line.substr(line.find("\"", pos + 1) + 1);
                std::string quarantine_path = std::string(QUARANTINE_PATH) + "\\" + hash;
                std::string action;
                switch (get_setting("virus_ctrl:virus_found:action")) {
                case 1: // remove
                    if (std::remove(path.c_str()) != 0) {
                        action = "remove_failed";
                        log(LOGLEVEL::ERR, "[virus_ctrl_process()]: Error while removing infected file: ", path, " ", hash);
                    }
                    else {
                        log(LOGLEVEL::VIRUS, "[virus_ctrl_process()]: Removed Virus: ", path, " ", hash, "");
                        action = "removed";
                    }
                    break;

                case 2: // quarantine
                    if (std::rename(path.c_str(), quarantine_path.c_str()) != 0) {
                        log(LOGLEVEL::ERR, "[virus_ctrl_process()]: Error while quarantining infected file: ", path, " ", hash);
                        action = "quarantine_failed";
                    }
                    else {
                        log(LOGLEVEL::VIRUS, "[virus_ctrl_process()]: Quarantined Virus: ", path, " ", hash, " to ", quarantine_path);
                        action = "quarantined";
                    }
                    break;

                case 3: // ignore
                    // Ignore this file and just continue. But for good measure, we should log it.
                    log(LOGLEVEL::VIRUS, "[virus_ctrl_process()]: Virus found in file: ", path, " ", hash, " but ignored due to settings");
                    action = "ignored";
                    break;

                case 4: // notify
                    // We should also log it.
                    log(LOGLEVEL::VIRUS, "[virus_ctrl_process()]: Virus found in file: ", path, " ", hash, " but only notified due to settings");
                    action = "notified";
                    break;
                }

                // Notify the server
                std::string url;
                url = get_setting_string("server:server_url");
                if(url != "nan" && url != "") {
                    url += "/api/php/virus/notify_virus.php?";
                    url += "file=" + url_encode(path);
                    url += "&hash=" + url_encode(hash);
                    url += "&action=" + action;
                    url += "&machine_id=" + get_machineid(SECRETS);
                    url += "&apikey=" + get_apikey(SECRETS);
                    //send notification using fastsend
                    std::thread send(fast_send, url, get_setting("communication:unsafe_tls"));
                    send.detach();
                    Sleep(10);//in order to wait for the thread to copy the params into its own memory
                }
                else {
                    log(LOGLEVEL::ERR, "[virus_ctrl_process()]: Error while notifying server about virus: ", path, " ", hash);
                }
                //notify desktop client
                std::ofstream answer_com(ANSWER_COM_PATH,std::ios::app);
                if (answer_com.is_open()) {
                    answer_com << "found " << "\"" << path << "\"" << " " << hash << " " << action << "\n";
                    answer_com.close();
                }

            }
        }
        file.close();
    }
    std::remove((VIRUS_CTRL_DB + id).c_str());
    return 0;
}


#endif
