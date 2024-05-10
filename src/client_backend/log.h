#pragma once
#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>
#include "connect.h"
#include "well_known.h"
#include "settings.h"
#include "security.h"

enum class LOGLEVEL {
    INFO,
    WARN,
    ERR,
    VIRUS,
    RISK,
    PANIC,
    INFO_NOSEND,
    WARN_NOSEND,
    ERR_NOSEND,
    PANIC_NOSEND
};

std::string get_loglevel(LOGLEVEL level);

template <typename... Args>
void log(LOGLEVEL level, const std::string& message, Args&&... args) {
    log_timeout_reset_set(log_timeout_reset_get() + 1);
    std::string prefix = get_loglevel(level);
    std::time_t now = std::time(nullptr);
    std::tm tm;
    localtime_s(&tm, &now);
    int error = 0;
    std::ostringstream logStream;
    std::ostringstream to_srv;
    to_srv << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << ";" << prefix << ";" << message;
    logStream << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " " << prefix << " " << message;
    if constexpr (sizeof...(args) > 0) {
        ((logStream << ' ' << std::forward<Args>(args)), ...);
        ((to_srv << ' ' << std::forward<Args>(args)), ...);
    }
    logStream << std::endl;
    std::string logString = logStream.str();
    std::string to_srv_string = to_srv.str();
    printf("info from logger: %s", logString.c_str());
    FILE* fp;
    switch (level) {
    case LOGLEVEL::INFO:
        error = fopen_s(&fp, INFOFILE, "a");
        break;
    case LOGLEVEL::WARN:
        error = fopen_s(&fp, WARNFILE, "a");
        break;
    case LOGLEVEL::ERR:
        error = fopen_s(&fp, ERRORFILE, "a");
        break;
    case LOGLEVEL::VIRUS:
        error = fopen_s(&fp, VIRUSFILE, "a");
        break;
    case LOGLEVEL::RISK:
        error = fopen_s(&fp, RISKFILE, "a");
        break;
    case LOGLEVEL::PANIC:
        error = fopen_s(&fp, PANICFILE, "a");
        break;
    case LOGLEVEL::INFO_NOSEND:
        error = fopen_s(&fp, INFOFILE, "a");
        break;
    case LOGLEVEL::WARN_NOSEND:
        error = fopen_s(&fp, WARNFILE, "a");
        break;
    case LOGLEVEL::ERR_NOSEND:
        error = fopen_s(&fp, ERRORFILE, "a");
        break;
    default:
        error = fopen_s(&fp, LOGFILE, "a");
        break;
    }
    if (error != 0) {
        return;
    }
    else {
        fprintf_s(fp, "%s", logString.c_str());
        fclose(fp);
        if (fopen_s(&fp, LOGFILE, "a") == 0) {
            fprintf_s(fp, "%s", logString.c_str());
            fclose(fp);
        }
        if (fopen_s(&fp, SRV_LOGFILE, "a") == 0 && log_timeout_get() >= 5) {
            fprintf_s(fp, "%s\n", to_srv_string.c_str());
            fclose(fp);
        }
    }
    //printf("info from logger2: %s", logString.c_str());
    if (level != LOGLEVEL::INFO_NOSEND && level != LOGLEVEL::WARN_NOSEND && level != LOGLEVEL::ERR_NOSEND && level != LOGLEVEL::PANIC_NOSEND/* && log_timeout_get() < 5*/) {
        std::string url;
        int res = 0;
        url = get_setting_string("server:server_url");
        if (url!="" && url!="nan") {
            url += "/api/php/log/add_entry.php?logtext=";
            url += url_encode(to_srv_string);
            url += "&machine_id=";
            url += get_machineid(SECRETS);
            url += "&apikey=";
            url += get_apikey(SECRETS);
            //printf("sending to srv: %s", url.c_str());
            //send with fastsend in new thread
            std::thread send_thread(fast_send, url, get_setting("communication:unsafe_tls"));
            send_thread.detach();
            Sleep(10);//in order to wait for the thread to copy the params into its own memory
        }
    }


    if (log_timeout_reset_get() > 100)
    {
        log_timeout_reset_set(0);
        log_timeout_set(0);
    }
}

#endif // LOGGER_H
