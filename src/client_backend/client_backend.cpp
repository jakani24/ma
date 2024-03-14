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
#include "check_process.h"
int main() {
    //log(LOGLEVEL::INFO, "[main()]:Starting main thread.");
    //return 0;

    log(LOGLEVEL::INFO_NOSEND, "[main()]:Starting main thread.");
    int err = 0;
	printf("welcome to the jakach security tool main thread\n");
    //exit(0);
    if (load_settings() == 0) {//load the settings from the settings file
        if (update_settings("settings")!=0) { //update the settings from the server
            log(LOGLEVEL::ERR_NOSEND, "[main()]:Could not update settings (settings) from server.");
        }
        if (update_settings("rtp_included")!=0) { //update the settings from the server
            log(LOGLEVEL::ERR_NOSEND, "[main()]:Could not update settings (rtp_included) from server.");
        }
        if (update_settings("rtp_excluded")!=0) { //update the settings from the server
            log(LOGLEVEL::ERR_NOSEND, "[main()]:Could not update settings (rtp_excluded) from server.");
        }
        if (update_settings("sched")!=0) { //update the settings from the server
            log(LOGLEVEL::ERR_NOSEND, "[main()]:Could not update settings (scheduled_tasks) from server.");
        }
        load_settings(); //load the updated settings from the settings file
    }
    else {
		log(LOGLEVEL::ERR_NOSEND, "[main()]:Could not load settings from file.");
        log(LOGLEVEL::PANIC_NOSEND, "[main()]:Panic, no settings file loaded, terminating process!");
        exit(1);
	}

    // Initialize hash databases
    err = initialize(DB_DIR);
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

    // Main thread loop
    while (!app_stop()) {
        auto start = std::chrono::high_resolution_clock::now();

        // Check for tasks from user interface
        if (check_for_com_tasks(MAIN_COM, MAIN_COM_PATH) != 0) {
            // Log message commented out as this error is expected when the file doesn't exist
           // log(LOGLEVEL::ERR, "[main()]:Error opening communication file in: ", MAIN_COM_PATH);
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

    return 0;
}
