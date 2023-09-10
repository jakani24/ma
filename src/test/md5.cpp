#include <iostream>
#include <fstream>
#include <openssl/md5.h>

int main() {
    // Open the file for reading
    std::ifstream file("your_file_path", std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }

    // Initialize the MD5 context
    MD5_CTX md5Context;
    MD5_Init(&md5Context);

    // Read the file in chunks and update the MD5 context
    char buffer[1024];
    while (file.read(buffer, sizeof(buffer))) {
        MD5_Update(&md5Context, buffer, file.gcount());
    }

    // Finalize the MD5 hash
    unsigned char md5Digest[MD5_DIGEST_LENGTH];
    MD5_Final(md5Digest, &md5Context);

    // Print the MD5 hash
    std::cout << "MD5 Hash: ";
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        printf("%02x", md5Digest[i]);
    }
    std::cout << std::endl;

    // Close the file
    file.close();

    return 0;
}
