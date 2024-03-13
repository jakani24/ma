#ifndef LOCAL_COM_CPP
#define LOCAL_COM_CPP
#include "local_com.h"

#include <iostream>
#include <fstream>
#include <string>

int check_for_com_tasks(const std::string& com_name, const std::string& com_path) {
    std::ifstream file(com_path);

    if (!file.is_open()) {
        // No com file found = no communication needed
        return 1;
    }

    std::string command;
    while (file >> command) {
        // Scan the command
        std::string path;
        file >> path; // Scan the path

        // Add command and path to the queue
        std::string queue_entry = command + ";" + path;
        queue_push(queue_entry.c_str());
    }

    // Remove the com file after processing
    file.close();
    std::remove(com_path.c_str());

    return 0;
}
#endif // !LOCAL_COM_CPP
