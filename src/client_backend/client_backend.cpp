#include <iostream>
#include <curl/curl.h>
#include <openssl/md5.h>
#include "md5hash.h"
int main() {
	char md5Hash[2 * MD5_DIGEST_LENGTH + 1]; // +1 for null-terminator
	md5_file("C:\\Users\\janis\\Documents\\Projekte_mit_c\\ma\\ma\\src\\client_backend\\x64\\Debug\\client_backend.exe", md5Hash);
	printf("%s", md5Hash);

    CURL* curl = curl_easy_init();

    if (!curl) {
        std::cerr << "Failed to initialize libcurl." << std::endl;
        return 1;
    }

    std::cout << "libcurl is correctly installed and initialized." << std::endl;

    // Clean up libcurl
    curl_easy_cleanup(curl);

	return 0;
}