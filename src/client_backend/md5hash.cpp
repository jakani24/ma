#pragma warning(disable:4996)
#include "md5hash.h"

int md5_file(const char*path,char*md5Hash) {
    std::ifstream file(path, std::ios::binary);

    if (!file) {
        std::cerr << "Error opening file." << std::endl;
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