#include "security.h"
#include <mutex> // Include the mutex header

std::mutex fileMutex_sec; // Mutex to synchronize file access

int check_cert(const char* cert, const char* secrets_path) {
    FILE* fp;
    if (fopen_s(&fp, secrets_path, "r") != 0) {
        return 1;
    }
    else {
        char secrets[505]; // Allocate memory on the stack
        std::lock_guard<std::mutex> lock(fileMutex_sec); // Lock file access

        while (!feof(fp)) {
            fscanf_s(fp, "%s", secrets, 500); // get the secret
            if (strcmp("cert", secrets) == 0) {
                fscanf_s(fp, "%s", secrets, 500); // get the secret
                if (strcmp(cert, secrets) == 0) {
                    fclose(fp);
                    return 0;
                }
            }
        }
        fclose(fp);
        return 2;
    }
}

char* get_apikey(const char* secrets_path) {
    FILE* fp;
    if (fopen_s(&fp, secrets_path, "r") != 0) {
        return nullptr;
    }
    else {
        char secrets[505]; // Allocate memory on the stack
        std::lock_guard<std::mutex> lock(fileMutex_sec); // Lock file access

        while (!feof(fp)) {
            fscanf_s(fp, "%s", secrets, 500); // get the secret
            if (strcmp("apikey", secrets) == 0) {
                fscanf_s(fp, "%s", secrets, 500); // get the secret
                fclose(fp);
                return secrets; // Return a dynamically allocated copy
            }
        }
        fclose(fp);
        return nullptr;
    }
}

char* get_machineid(const char* secrets_path) {
    FILE* fp;
    if (fopen_s(&fp, secrets_path, "r") != 0) {
        return nullptr;
    }
    else {
        char secrets[300]; // Allocate memory on the stack
        std::lock_guard<std::mutex> lock(fileMutex_sec); // Lock file access

        while (!feof(fp)) {
            fscanf_s(fp, "%s", secrets, 295); // get the secret
            if (strcmp("machineid", secrets) == 0) {
                fscanf_s(fp, "%s", secrets, 295); // get the secret
                fclose(fp);
                return secrets; // Return a dynamically allocated copy
            }
        }
        fclose(fp);
        return nullptr;
    }
}
