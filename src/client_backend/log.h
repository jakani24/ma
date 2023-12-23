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
    ERR
};

std::string get_loglevel(LOGLEVEL level);

template <typename... Args>
void log(LOGLEVEL level, const std::string& message, Args&&... args) {
    std::string prefix = get_loglevel(level);
    std::time_t now = std::time(nullptr);
    std::tm tm = *std::localtime(&now);

    std::ostringstream logStream;
    logStream << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " " << prefix << " " << message;
    if constexpr (sizeof...(args) > 0) {
        ((logStream << ' ' << std::forward<Args>(args)), ...);
    }
    logStream << std::endl;
    std::string logString = logStream.str();

    // Open the file based on log level
    std::ofstream logFile;
    switch (level) {
    case LOGLEVEL::INFO:
        logFile.open(INFOFILE, std::ios_base::app);
        break;
    case LOGLEVEL::WARN:
        logFile.open(WARNFILE, std::ios_base::app);
        break;
    case LOGLEVEL::ERR:
        logFile.open(ERRORFILE, std::ios_base::app);
        break;
    }

    // Write the log to the file
    if (logFile.is_open()) {
        logFile << logString.c_str();
        logFile.close();
    }
    //write the log to the general file
    logFile.open(LOGFILE, std::ios_base::app);
    if (logFile.is_open()) {
        logFile << logString.c_str();
        logFile.close();
    }
}

#endif // LOGGER_H
