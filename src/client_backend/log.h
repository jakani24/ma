#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "well_known.h"
enum class LOGLEVEL {
    INFO,
    WARN,
    ERR,
    VIRUS,
    RISK,
    PANIC
};

std::string get_loglevel(LOGLEVEL level);

template <typename... Args>
void log(LOGLEVEL level, const std::string& message, Args&&... args) {
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
    to_srv << std::endl;
    std::string logString = logStream.str();
    std::string to_srv_string = to_srv.str();
    printf("info from logger: %s", logString.c_str());
    // Open the file based on log level
    FILE* fp;
    switch (level) {
    case LOGLEVEL::INFO:
        error=fopen_s(&fp, INFOFILE, "a");
        break;
    case LOGLEVEL::WARN:
        error=fopen_s(&fp, WARNFILE, "a");
        break;
    case LOGLEVEL::ERR:
        error=fopen_s(&fp, ERRORFILE, "a");
        break;
    case LOGLEVEL::VIRUS:
        error=fopen_s(&fp, VIRUSFILE, "a");
		break;
    case LOGLEVEL::RISK:
		error=fopen_s(&fp, RISKFILE, "a");
		break;
    case LOGLEVEL::PANIC:
        error = fopen_s(&fp, PANICFILE, "a");
        break;

	default:
		error=fopen_s(&fp, LOGFILE, "a");
		break;
	}
	if (error != 0) {
		//panic, create log entry, return 1;
        //printf("a");
		return;
    }
    else {
        switch (level) {
        case LOGLEVEL::INFO:
            fprintf_s(fp, "%s", logString.c_str());
            break;
        case LOGLEVEL::WARN:
            fprintf_s(fp, "%s", logString.c_str());
            break;
        case LOGLEVEL::ERR:
            fprintf_s(fp, "%s", logString.c_str());
            break;
        case LOGLEVEL::VIRUS:
            fprintf_s(fp, "%s", logString.c_str());
            break;
        case LOGLEVEL::RISK:
            fprintf_s(fp, "%s", logString.c_str());
            break;
        case LOGLEVEL::PANIC:
            fprintf_s(fp, "%s", logString.c_str());
            break;

        default:
            fprintf_s(fp, "%s", logString.c_str());
            break;
        }
        fclose(fp);
        if (fopen_s(&fp, LOGFILE, "a") == 0) {
            fprintf_s(fp, "%s", logString.c_str());
            fclose(fp);
        }
        if (fopen_s(&fp, SRV_LOGFILE, "a") == 0) {
            fprintf_s(fp, "%s", to_srv_string.c_str());
            fclose(fp);
        }
    }
}

#endif // LOGGER_H
