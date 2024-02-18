#ifndef LOG_CPP
#define LOG_CPP
#include "log.h"

std::string get_loglevel(LOGLEVEL level) {
    switch (level) {
    case LOGLEVEL::INFO:
        return "INFO";
    case LOGLEVEL::WARN:
        return "WARNING";
    case LOGLEVEL::ERR:
        return "ERROR";
    case LOGLEVEL::VIRUS:
		return "VIRUS";
    case LOGLEVEL::RISK:
        return "RISK";
    case LOGLEVEL::PANIC:
        return "PANIC";

    default:
        return "UNKNOWN";
    }
}


#endif