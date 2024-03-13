#include "security.h"
#include <mutex> // Include the mutex header
#include <string> // Include the string header
#include <fstream> // Include the file stream header

std::mutex fileMutex_sec; // Mutex to synchronize file access

int check_cert(const std::string& cert, const std::string& secrets_path) {
    std::ifstream file(secrets_path);
    if (!file.is_open()) {
        return 1;
    }
    else {
        std::string secrets;
        std::lock_guard<std::mutex> lock(fileMutex_sec); // Lock file access

        while (file >> secrets) {
            if (secrets == "cert") {
                file >> secrets; // Get the secret
                if (cert == secrets) {
                    return 0;
                }
            }
        }
        return 2;
    }
}

std::string get_apikey(const std::string& secrets_path) {
    std::ifstream file(secrets_path);
    if (!file.is_open()) {
        return "";
    }
    else {
        std::string secrets;
        std::lock_guard<std::mutex> lock(fileMutex_sec); // Lock file access

        while (file >> secrets) {
            if (secrets == "apikey") {
                file >> secrets; // Get the secret
                return secrets; // Return the secret
            }
        }
        return "";
    }
}

std::string get_machineid(const std::string& secrets_path) {
    std::ifstream file(secrets_path);
    if (!file.is_open()) {
        return "";
    }
    else {
        std::string secrets;
        std::lock_guard<std::mutex> lock(fileMutex_sec); // Lock file access

        while (file >> secrets) {
            if (secrets == "machineid") {
                file >> secrets; // Get the secret
                return secrets; // Return the secret
            }
        }
        return "";
    }
}
