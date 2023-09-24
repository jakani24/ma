#pragma warning(disable:4996)
#include <iostream>
#include <curl/curl.h>
#include <openssl/md5.h>
#include <yara.h>
#include "md5hash.h"
#include "connect.h"
#include "scan.h"
int main() {
	printf("welcome to the jakach security tool\n");
	char md5Hash[2 * MD5_DIGEST_LENGTH + 1]; // +1 for null-terminator
	printf("Hash of the executable: ");
	md5_file("C:\\Users\\janis\\Documents\\Projekte_mit_c\\ma\\ma\\src\\client_backend\\x64\\Debug\\client_backend.exe", md5Hash);
	printf("%s", md5Hash);
    char a[2000];
    printf("\nerror:%d\n",connect_to_srv("https://self-signed.badssl.com/", a, 2000,1)); //error 60: self signed => option für self-signed ignorieren aktivieren (bool ignore_invalid=true)
    printf("%s", a);																	 //error 6: not reachable
	return 0;
}

