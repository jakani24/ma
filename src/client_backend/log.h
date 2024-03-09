#pragma once
#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
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
    log_timeout_reset_set(log_timeout_reset_get()+1);
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
    //to_srv << std::endl;
    std::string logString = logStream.str();
    std::string to_srv_string = to_srv.str();
    printf("info from logger: %s", logString.c_str());
    // Open the file based on log level
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
        //panic, create log entry, return 1;
        return;
    }
    else {
        //write log entry to disk
        fprintf_s(fp, "%s", logString.c_str());
        fclose(fp);

        //write to general log file
        if (fopen_s(&fp, LOGFILE, "a") == 0) {
            fprintf_s(fp, "%s", logString.c_str());
            fclose(fp);
        }
        //write to server log file only if we werent able to send the logs directly. this file will store them until we can upload them
        if (fopen_s(&fp, SRV_LOGFILE, "a") == 0 && log_timeout_get() >= 5) { //if server already did not respon over 5 times, we add it to the log file
            fprintf_s(fp, "%s\n", to_srv_string.c_str());
            fclose(fp);
        }
    }
    //send log so srv
    //build up the log string: loglevel&logtext&machineid&date
    //to_srv_string=includes the log message
    //we now need to build up the request string and append the machineid
    if (level!=LOGLEVEL::INFO_NOSEND && level!=LOGLEVEL::WARN_NOSEND && level!=LOGLEVEL::ERR_NOSEND && level!=LOGLEVEL::PANIC_NOSEND && log_timeout_get()<5) {
        char* url = new char[3000];
        int res = 0;
        if (get_setting("server:server_url", url) == 0 or strcmp(url, "nan") == 0) {
            strcat_s(url, 3000, "/api/php/log/add_entry.php?logtext=");//need to add machine_id and apikey
            strcat_s(url, 3000, url_encode(to_srv_string.c_str()));
            strcat_s(url, 3000, "&machine_id=");
            strcat_s(url, 3000, get_machineid(SECRETS));
            strcat_s(url, 3000, "&apikey=");
            strcat_s(url, 3000, get_apikey(SECRETS));
            res=fast_send(url, get_setting("communication:unsafe_tls"));
            if (res != 0) {
                //we know that the server might be down, so we will increment the timeout counter
                log_timeout_set(log_timeout_get() + 1);
            }
            //we might not want to log an error occuring here because it will create a loop
        }
        delete[] url;
    }//else we do not send the log to the server
    if (log_timeout_reset_get() > 100)
    {
        log_timeout_reset_set(0);
        log_timeout_set(0);
    }
    //!!!i disabled this temporarely, because it does not really work and as long as the client is in lan the other code works anyway
    if(log_timeout_reset_get()>100 && 1==0) { //after 100 log entrys, we cna try again to reach out ot the server.
        log_timeout_reset_set(0);
        log_timeout_set(0);
        //try to upload the server_log file, where we stored the logs which we could not upload.
        char* url = new char[3000];
        int res = 0;
        if (get_setting("server:server_url", url) == 0 or strcmp(url, "nan") == 0) {
			strcat_s(url, 3000, "/api/php/log/add_log.php?machine_id=");
			strcat_s(url, 3000, get_machineid(SECRETS));
			strcat_s(url, 3000, "&apikey=");
			strcat_s(url, 3000, get_apikey(SECRETS));
            res=upload_to_srv(SRV_LOGFILE, url, get_setting("communication:unsafe_tls"));
            if (res != 0) {
				//we know that the server might be down, so we will increment the timeout counter
				log_timeout_set(log_timeout_get() + 1);
            }
            else {
                //remove the logfile
                remove(SRV_LOGFILE);
            }
		}
        delete [] url;
	}
    
}

#endif // LOGGER_H
