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
void log(LOGLEVEL level, const std::string& message, Args&&... args);


#endif // LOGGER_H
