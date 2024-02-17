#pragma warning(disable:4996)
#include <iostream>
#include <thread>
#include <curl/curl.h>
#include <openssl/md5.h>
//#include <yara.h>
#include "app_ctrl.h"
#include "md5hash.h"
#include "connect.h"
#include "scan.h"
#include "queue_ctrl.h"
#include "well_known.h"
#include "local_com.h"
#include "local_schedule.h"
#include "log.h"
#include "thread_ctrl.h"
#include "settings.h"
#include "check_dir.h"
#include "virus_ctrl.h"
#include "update.h"
int main() {
    log(LOGLEVEL::INFO, "[main()]:Starting main thread.");
	printf("welcome to the jakach security tool main thread\n");
    load_settings();//load the settings from the settings file
    if (update_settings("settings")) { //update the settings from the server
        log(LOGLEVEL::ERR, "[main()]:Could not update settings from server.");
    }
    if (update_settings("rtp_included")) { //update the settings from the server
        log(LOGLEVEL::ERR, "[main()]:Could not update settings from server.");
    }
    if (update_settings("rtp_excluded")) { //update the settings from the server
        log(LOGLEVEL::ERR, "[main()]:Could not update settings from server.");
    }
    if (update_settings("sched")) { //update the settings from the server
        log(LOGLEVEL::ERR, "[main()]:Could not update settings from server.");
    }
    load_settings();
    initialize(DB_DIR); //load the hash databases into memory

    //start a second thread which will scan for new files
    if (get_setting("rtp_folder_scan:status") == 1) {
        log(LOGLEVEL::INFO, "[main()]:Starting real time protection.");
        std::thread folder_scannner_thread(folder_scanner);
        folder_scannner_thread.detach();
    }
 
    //main thread:
    /* watches for notifications on bus
    * start threads (scans etc); only one at a time may run
    * updates settings etc
    * start scheduled tasks
    
    
    */
    while (!app_stop()) {
        //run all the tasks described above
        //check for tasks in com
        //check for scheduled tasks
        //execute tasks
        auto start = std::chrono::high_resolution_clock::now();
        check_for_com_tasks(MAIN_COM, MAIN_COM_PATH); //check for tasks from user interface and add them to the queue
        check_for_sched_tasks(SCHED, SCHED_PATH);   //check for scheduled tasks and add them to the queue
        //unlock_task("tsk1"); else it will only be executed once. but this function has to be called at the end of the task. else it will nvr be executed again. this would be bad :(
        //start a thread that executes check_scan_dir to scan folders for new files. this thread then should start a ock so only one scanfolder thread runs at a time
        //Sleep(1000);

        if (can_run_thread()) {
            int queue_size = get_queue_size();
            for (int i = 0; i < queue_size; i++) {
                char* queue_entry = new char[300 * 2 + 5];
                queue_entry[0] = '\0';
                queue_pop(queue_entry);
                //execute the function which starts the threads
                start_thread(queue_entry);
                delete[] queue_entry;
            }
        }
        //to ensure that the loop takes at least 1 second else it will run too fast nd destroy you CPU :)
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        if (duration.count() < 1000)
            Sleep(1000 - duration.count());
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

