/*
This file contains the implementation of the md5hash.h header file. It containes the functions to hash files using the MD5 algorithm. It also contains a mutex for thread synchronization.

Functions:
	- md5_file(): This function hashes a file using the MD5 algorithm.
	- md5_file_t(): This function hashes a file using the MD5 algorithm. It is thread-safe.
*/

#pragma warning(disable:4996)
#include "md5hash.h"
#include "log.h"
#include <mutex> // Include the mutex header
#include <fstream> // Include the fstream header

// Define a mutex for thread synchronization
std::mutex fileMutex_hash;

// Function to hash a file using the MD5 algorithm
int md5_file(const char* path, char* md5Hash) {
    std::ifstream file(path, std::ios::binary);

    if (!file) {
        return 1;
    }

    // Initialize OpenSSL's MD5 context
    MD5_CTX md5Context;
    MD5_Init(&md5Context);

    // Read and update the context with the file's content
    char buffer[1024];
    while (file.good()) {
        file.read(buffer, sizeof(buffer));
        MD5_Update(&md5Context, buffer, file.gcount());
    }

    // Finalize the MD5 hash and store it in result
    unsigned char result[MD5_DIGEST_LENGTH];
    MD5_Final(result, &md5Context);

    // Close the file
    file.close();

    // Store the MD5 hash in a char array
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        snprintf(&md5Hash[i * 2], 3, "%02x", result[i]);
    }
    return 0;
}

// Function to hash a file using the MD5 algorithm (thread-safe)
std::string md5_file_t(const std::string& path_) {
    std::string path(path_);
    FILE* fp = fopen(path.c_str(), "rb");
    if (!fp) {
        return "nan"; // Return the path because otherwise, it might crash (if we send a nullptr)
    }

    // Lock access to the file
    std::lock_guard<std::mutex> lock(fileMutex_hash);

    // Initialize OpenSSL's MD5 context
    MD5_CTX md5Context;
    MD5_Init(&md5Context);

    // Read and update the context with the file's content
    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, 1024, fp)) > 0) {
        MD5_Update(&md5Context, buffer, bytesRead);
    }

    // Finalize the MD5 hash and store it in result
    unsigned char result[MD5_DIGEST_LENGTH];
    MD5_Final(result, &md5Context);

    // Close the file
    fclose(fp);

    // Convert the MD5 hash to a string using snprintf
    char md5Hash[2 * MD5_DIGEST_LENGTH + 1];
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        snprintf(&md5Hash[i * 2], 3, "%02x", result[i]);
    }
    return std::string(md5Hash);
}
