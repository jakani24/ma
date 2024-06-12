/*
This file is the main file for the client backend. It is responsible for starting the main thread and initializing all the other threads and functions. It also contains the main loop for the main thread which checks for tasks from the user interface, scheduled tasks, and tasks from the queue. It also initializes the hash databases, the yara rules, and the real-time protection threads. The main thread will run until the app_stop() function returns true, which is set by the user interface when the user wants to stop the application.

Functions:
- main(): The main function of the client backend. It initializes the hash databases, yara rules, and real-time protection threads. It then enters a loop where it checks for tasks from the user interface, scheduled tasks, and tasks from the queue. It will run until the app_stop() function returns true.
*/

#pragma warning(disable:4996)
#include <iostream>
#include <thread>
#include <windows.h>
#include <wininet.h>
#include <curl/curl.h>
#include <openssl/md5.h>
#include <yara.h>
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
#include "check_process.h"
#include "utils.h"
#include "deepscan.h"


int main(int argc, char* argv[]) {
    //printf("%d",InternetCheckConnection("https://www.google.com", FLAG_ICC_FORCE_CONNECTION, 0));
    log(LOGLEVEL::INFO_NOSEND, "[main()]:Starting main thread.");
    int err = 0;
    printf("welcome to the jakach security tool main thread\n");
    if (load_settings() == 0) {//load the settings from the settings file
        if (argc != 2) {
            if (update_settings("settings") != 0) { //update the settings from the server
                log(LOGLEVEL::ERR_NOSEND, "[main()]:Could not update settings (settings) from server.");
            }
            if (update_settings("rtp_included") != 0) { //update the included paths database for the real time proccess scanner from the server
                log(LOGLEVEL::ERR_NOSEND, "[main()]:Could not update settings (rtp_included) from server.");
            }
            if (update_settings("rtp_excluded") != 0) {//update the excluded paths database for the real time proccess scanner from the server
                log(LOGLEVEL::ERR_NOSEND, "[main()]:Could not update settings (rtp_excluded) from server.");
            }
            if (update_settings("sched") != 0) { //update the settings for the scheduler from the server
                log(LOGLEVEL::ERR_NOSEND, "[main()]:Could not update settings (scheduled_tasks) from server.");
            }
            if (update_settings("disallowed_start") != 0) { //update the settings for applicaiton control from the server
                log(LOGLEVEL::ERR_NOSEND, "[main()]:Could not update settings (disallowed_start) from server.");
            }
            load_settings(); //load the updated settings from the settings file
        }
    }
    else {
        log(LOGLEVEL::ERR_NOSEND, "[main()]:Could not load settings from file.");
        log(LOGLEVEL::PANIC_NOSEND, "[main()]:Panic, no settings file loaded, terminating process!");
        Sleep(1000); //wait for the log to be written
        exit(1);
    }

    //do self check
    if ((err = selfcheck()) != 0) {
        log(LOGLEVEL::PANIC, "[main()]:This installation of cyberhex failed the self check! Application may be tampered with!", err);
        log(LOGLEVEL::PANIC, "[main()]:Panic, self check failed, terminating process!");
        Sleep(1000); //wait for the log to be written and swnt to the server
        exit(1);
    }


    //init debug mode if needed
    if (argc == 2) {
        if (strcmp(argv[1], "--debug") == 0) {
            debug_mode_init();
        }
    }

    // Initialize hash databases
    err = initialize(DB_DIR);
    log(LOGLEVEL::INFO_NOSEND, "[main()]:Hash databases initialized.");
    if (err != 0) {
        switch (err) {
        case 1:
            log(LOGLEVEL::ERR, "[main()]:Error opening database file in: ", DB_DIR);
            break;
        case 2:
            log(LOGLEVEL::ERR, "[main()]:Error creating database file mapping in: ", DB_DIR);
            break;
        case 3:
            log(LOGLEVEL::ERR, "[main()]:Error mapping database file in: ", DB_DIR);
            break;
        default:
            log(LOGLEVEL::ERR, "[main()]:Unknown error while loading database file in: ", DB_DIR);
            break;
        }
    }

    // Start a second thread for real-time protection
    if (get_setting("rtp_folder_scan:status") == 1) {
        log(LOGLEVEL::INFO, "[main()]:Starting real time file protection.");
        std::thread folder_scanner_thread(folder_scanner);
        folder_scanner_thread.detach();
    }
    if (get_setting("rtp_process_scan:status") == 1) {
        log(LOGLEVEL::INFO, "[main()]:Starting real time process protection.");
        std::thread process_scanner_thread(process_scanner);
        process_scanner_thread.detach();
    }


    //initialize the deep scan database
    yr_initialize();
    init_yara_rules(YARA_DB_DIR);
    log(LOGLEVEL::INFO_NOSEND, "[main()]:Yara rules initialized.");

    // Main thread loop
    while (!app_stop()) {
        auto start = std::chrono::high_resolution_clock::now();

        // Check for tasks from user interface
        //printf("checking for tasks from user interface\n");
        if (check_for_com_tasks(MAIN_COM, MAIN_COM_PATH) != 0) {
            // Log message commented out as this error is expected when the file doesn't exist
        }
        // Check for scheduled tasks
        if (check_for_sched_tasks(SCHED, SCHED_PATH) != 0) {
            log(LOGLEVEL::ERR, "[main()]:Error opening schedule file in: ", SCHED_PATH);
        }
        // Execute tasks from the queue
        if (can_run_thread()) {
            int queue_size = get_queue_size();
            for (int i = 0; i < queue_size; i++) {
                start_thread(queue_pop());
            }
        }
        // Sleep to ensure loop takes at least 1 second
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        if (duration.count() < 1000)
            Sleep(1000 - duration.count());
    }
    yr_finalize();


    return 0;
}
