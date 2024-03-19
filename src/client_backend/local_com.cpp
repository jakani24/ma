#ifndef LOCAL_COM_CPP
#define LOCAL_COM_CPP
#include "local_com.h"
#include "connect.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>


int check_for_com_tasks(const std::string& com_name, const std::string& com_path) {
    /*std::ifstream file(com_path);
    if (!file.is_open()) {
        return 1; // Error opening file
    }

    std::string command;
    std::string path;
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        if (ss >> command) {
            // Check if the next token is a quoted path
            if (line.find("\"") != std::string::npos) {
                size_t start = line.find("\"") + 1; // Start after the opening quote
                size_t end = line.find_last_of("\""); // Find the last quote
                if (start != std::string::npos && end != std::string::npos) {
                    path = line.substr(start, end - start);
                }
            }
            else {
                // If no quotes found, simply take the next token as the path
                ss >> path;
            }
            // Add command and path to the queue
            std::string queue_entry = command + ";" + path;
            queue_push(queue_entry.c_str());         
        }
    }

    file.close();
    std::remove(com_path.c_str());
    */
    std::string line;
    std::string command="";
    std::string path="";
    line=read_from_pipe();
    std::stringstream ss(line);
    if (ss >> command) {
        size_t start = line.find("\"") + 1; // Start after the opening quote
        size_t end = line.find_last_of("\""); // Find the last quote
        if (start != std::string::npos && end != std::string::npos) {
            path = line.substr(start, end - start);
        }
    }
    std::string queue_entry = command + ";" + path;
    queue_push(queue_entry.c_str());
    return 0;
}
#endif // !LOCAL_COM_CPP
