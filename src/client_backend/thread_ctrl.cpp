#ifndef THREAD_CTRL_CPP
#define THREAD_CTRL_CPP

#include "thread_ctrl.h"
#include "log.h"
#include "well_known.h"
#include "scan.h"
#include "app_ctrl.h"
#include "update.h"
#include "utils.h"

int start_thread(const std::string& command) {
    if (can_run_thread()) {
        std::string out1, out2;
        split(command, ';', out1, out2);
        log(LOGLEVEL::INFO_NOSEND, "[start_thread()]: command: ", out1, " arguments: ",out2, " call: ",command);
        // Determine what should be executed
        if (out1 == "scanfile") {
            log(LOGLEVEL::INFO, "[start_thread()]: starting scanfile with arguments: ", out2);
            // Start a new thread with the scanfile function
            std::thread t1(action_scanfile, out2);
            t1.detach();
        }
        else if (out1 == "scanfolder") {
            // Start a new thread with the scanfolder function
            log(LOGLEVEL::INFO, "[start_thread()]: starting scanfolder with arguments: ", out2);
            std::thread t1(action_scanfolder, out2);
            t1.detach();
        }
        else if (out1 == "update_settings") {
            // Start a new thread with the update_settings function
            log(LOGLEVEL::INFO, "[start_thread()]: starting update_settings with arguments: ", out2);
            std::thread t1(action_update_settings);
            t1.detach();
        }
        else if (out1 == "update_db") {
            // Start a new thread with the update_db function
            log(LOGLEVEL::INFO, "[start_thread()]: starting update_db with arguments: ", out2);
            std::thread t1(action_update_db);
            t1.detach();
        }
        else if (out1 == "update_system") {
            // Start a new thread with the update_db function
            log(LOGLEVEL::INFO, "[start_thread()]: starting update_system with arguments: ", out2);
            std::thread t1(update_system);
            t1.detach();
        }

        while (can_run_thread()) {
            // Delay a bit to wait until the thread is started
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    return 0;
}

#endif // THREAD_CTRL_CPP
