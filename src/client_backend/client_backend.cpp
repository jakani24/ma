#pragma warning(disable:4996)
#include <iostream>
#include <thread>
#include <curl/curl.h>
#include <openssl/md5.h>
//#include <yara.h>
#include "md5hash.h"
#include "connect.h"
#include "scan.h"
#include "app_ctrl.h"
#include "queue_ctrl.h"
int main() {
	printf("welcome to the jakach security tool main thread\n");
    //main thread:
    /* watches for notifications on bus
    * start threads (scans etc); only one at a time may run
    * updates settings etc
    * start scheduled tasks
    
    
    */
    while (!app_stop()) {
        //run all the tasks described above

    }





    //ListFilesRecursive("C:\\", 0);
	/*char md5Hash[2 * MD5_DIGEST_LENGTH + 1]; // +1 for null-terminator
	printf("Hash of the executable: ");
	md5_file("C:\\Users\\janis\\Documents\\Projekte_mit_c\\ma\\ma\\src\\client_backend\\x64\\Debug\\client_backend.exe", md5Hash);
	printf("%s", md5Hash);
    char a_[2000];
    printf("\nerror:%d\n",connect_to_srv("https://self-signed.badssl.com/", a_, 2000,1)); //error 60: self signed => option für self-signed ignorieren aktivieren (bool ignore_invalid=true)
    printf("%s", a_);																	 //error 6: not reachable
	download_file_from_srv("https://jakach.duckdns.org/php/login/v3/login.php", "c:\\programdata\\jakach\\out12.txt");
    /*
    const int numThreads = 12;
    std::thread threads[numThreads];

    for (int i = 0; i < numThreads; ++i) {
        threads[i] = std::thread(ListFilesRecursive, "C:\\Users\\janis\\Documents\\ma_av_tests",i);
    }

    // Join threads to wait for them to finish
    for (int i = 0; i < numThreads; ++i) {
        threads[i].join();
    }

    std::cout << "All threads have finished." << std::endl;
    */

    //printf("code:%d",scan_hash("C:\\Users\\janis\\Documents\\ma_av_tests\\OutputFile.txt", "1fddc13c02a79442c911a44b02ee0f58"));
    
	return 0;
}

