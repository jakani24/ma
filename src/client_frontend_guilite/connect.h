#pragma once
#include <iostream>
#include <iomanip>
#include <sstream>
#include <locale>
#include <string>
#include <curl/curl.h>

int send_to_pipe(const std::string& message);
std::string read_from_pipe();
