/*
This file contains the implementation of the security functions.

Functions:
- check_cert(): This function checks if the certificate is valid.
- get_apikey(): This function returns the API key.
- get_machineid(): This function returns the machine ID.
- selfcheck(): This function checks if the files have been tampered with.

*/

#include "security.h"
#include <mutex> // Include the mutex header
#include <string> // Include the string header
#include <fstream> // Include the file stream header
#include "md5hash.h"
#include "well_known.h"

std::mutex fileMutex_sec; // Mutex to synchronize file access



// Function to check if the certificate is valid. the certificate is stored in the first line of each settings file. we check them agains the one in the secrets file
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

// Function to return the API key from the secrets file
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

// Function to return the machine ID from the secrets file
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

// Function to check if the cyberhex files have been tampered with
bool selfcheck() {
    //hash all the cyberhex files and check if they have been tampered with
    if (md5_file_t(DLL_LIBCRYPTO_PATH) != DLL_LIBCRYPTO_HASH)
        return 1;
    if(md5_file_t(DLL_LIBCURL_PATH)!=DLL_LIBCURL_HASH)
        return 2;
    if(md5_file_t(DLL_MSVCP140_PATH)!=DLL_MSVCP140_HASH)
        return 3;
    if(md5_file_t(DLL_MSVCP140D_PATH)!=DLL_MSVCP140D_HASH)
        return 4;
    if(md5_file_t(DLL_VCRUNTIME140_PATH)!=DLL_VCRUNTIME140_HASH)
        return 5;
    if(md5_file_t(DLL_VCRUNTIME140D_PATH)!=DLL_VCRUNTIME140D_HASH)
        return 6;
    if(md5_file_t(DLL_VCRUNTIME140_1D_PATH)!=DLL_VCRUNTIME140_1D_HASH)
        return 7;
    if(md5_file_t(DLL_ZLIB1_PATH)!=DLL_ZLIB1_HASH)
        return 8;
    if(md5_file_t(DLL_UCRTBASED_PATH)!=DLL_UCRTBASED_HASH)
        return 9;
    //if(md5_file_t(APP_PATH)!=APP_HASH)
	//	return 10;
    return 0;
}