#pragma warning(disable:4996)
#include "md5hash.h"
#include "log.h"
int md5_file( const char*path, char*md5Hash) {
     std::ifstream file(path, std::ios::binary);

    if (!file) {
        log(LOGLEVEL::ERR, "[md5_file()]: Could not open file for scanning ",path);
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
    //char md5Hash[2 * MD5_DIGEST_LENGTH + 1]; // +1 for null-terminator

    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        snprintf(&md5Hash[i * 2], 3, "%02x", result[i]);
    }
}

//thread save implementation
/*
char* md5_file_t(const char*path) {

    //log(LOGLEVEL::INFO, "[md5_file_t()]: Calculating MD5 hash of file ", path_);
    FILE* fp = fopen(path, "rb");
    if (!fp) {
        log(LOGLEVEL::ERR, "[md5_file_t()]: Could not open file for scanning ", path);
        return nullptr;
    }

    // Initialize OpenSSL's MD5 context
    MD5_CTX md5Context;
    MD5_Init(&md5Context);

    // Read and update the context with the file's content
      char buffer[1024];
     size_t bytesRead;
    while ((bytesRead = fread(buffer, 1,1024, fp)) > 0) {
        MD5_Update(&md5Context, buffer, bytesRead);
    }

    // Finalize the MD5 hash and store it in result
     unsigned char result[MD5_DIGEST_LENGTH];
    MD5_Final(result, &md5Context);

    // Close the file
    fclose(fp);

    // Convert the MD5 hash to a string using snprintf
     char* md5Hash = (char*)malloc(2 * MD5_DIGEST_LENGTH + 1);
    if (!md5Hash) {
        return nullptr;  // Allocation failure
    }

    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        snprintf(&md5Hash[i * 2], 3, "%02x", result[i]);
    }
    //log(LOGLEVEL::INFO, "[md5_file_t()]: MD5 hash of file ", path, " is ", md5Hash);
    return md5Hash;
}
*/
//thread save implementation
std::string md5_file_t(const std::string& path_) {
    //log(LOGLEVEL::INFO, "[md5_file_t()]: Calculating MD5 hash of file ", path_);
    thread_local std::string path (path_);
    thread_local FILE* fp = fopen(path.c_str(), "rb");
    if (!fp) {
        log(LOGLEVEL::WARN, "[md5_file_t()]: Could not open file for scanning ", path.c_str());
        return path; //we return the path because else it moight crash ()if we sen da nullptr
    }

    // Initialize OpenSSL's MD5 context
    thread_local MD5_CTX md5Context;
    MD5_Init(&md5Context);

    // Read and update the context with the file's content
    thread_local char buffer[1024];
    thread_local size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, 1024, fp)) > 0) {
        MD5_Update(&md5Context, buffer, bytesRead);
    }

    // Finalize the MD5 hash and store it in result
    thread_local unsigned char result[MD5_DIGEST_LENGTH];
    MD5_Final(result, &md5Context);

    // Close the file
    fclose(fp);

    // Convert the MD5 hash to a string using snprintf
    thread_local char* md5Hash = (char*)malloc(2 * MD5_DIGEST_LENGTH + 1);
    if (!md5Hash) {
        return path;  // Allocation failure
    }

    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        snprintf(&md5Hash[i * 2], 3, "%02x", result[i]);
    }
    //log(LOGLEVEL::INFO, "[md5_file_t()]: MD5 hash of file ", path, " is ", md5Hash);
    return md5Hash;
}