#ifndef LOCAL_SCHEDULE_CPP
#define LOCAL_SCHEDULE_CPP
#include "local_schedule.h"
#include "queue_ctrl.h"
#include "log.h"
/*
* To do:
* read scheduled tasks form file
* process time and date to determine if task should be executed
* add task to queue
*/

// Map to store task execution states
std::map<std::string, bool> task_states;

bool is_valid_field(const std::string& field, int value) {
    if (field == "*") {
        return true;  // Wildcard matches any value
    }

    // Parse the field and check for matches
    std::istringstream iss(field);
    std::vector<std::string> tokens;
    std::string token;
    while (std::getline(iss, token, ',')) {
        if (std::find(token.begin(), token.end(), '-') != token.end()) {
            // Range of values
            int start, end;
            if (sscanf_s(token.c_str(), "%d-%d", &start, &end) == 2 && value >= start && value <= end) {
                return true;
            }
        }
        else {
            // Single value
            int singleValue;
            if (sscanf_s(token.c_str(), "%d", &singleValue) == 1 && value == singleValue) {
                return true;
            }
        }
    }

    return false;
}

bool is_task_due(const std::string& task_name, const std::string& cron_expression, const std::tm& current_time) {
    std::istringstream iss(cron_expression);
    std::vector<std::string> fields;
    std::string field;
    while (iss >> field) {
        fields.push_back(field);
    }

    // Check if the 'fields' vector has at least 5 elements
    if (fields.size() < 5) {
        // Handle the case where the cron expression is not well-formed
        std::cerr << "Error: Invalid cron expression." << std::endl;
        return false;
    }

    // Check if the task should be executed
    if (is_valid_field(fields[0], current_time.tm_min) &&
        is_valid_field(fields[1], current_time.tm_hour) &&
        is_valid_field(fields[2], current_time.tm_mday) &&
        is_valid_field(fields[3], current_time.tm_mon + 1) &&
        is_valid_field(fields[4], current_time.tm_wday + 1)) {

        // Check if the task has not already been executed
        if (!task_states[task_name]) {
            // Set the flag to indicate that the task has been executed
            task_states[task_name] = true;
            return true;
        }
    }
    else {
        // Reset the flag
        task_states[task_name] = false;
    }

    return false;
}
void unlock_task(const std::string& task_name) {
    // Unlock the task by setting its state to false
    task_states[task_name] = false;
}

int check_for_sched_tasks(const char* sched_name, const char* sched_path) {
    FILE* fp = nullptr;
    char* command = new char[505];
    if ((fopen_s(&fp, sched_path, "r")) != 0) {
        //panic, create log entry, return 1;
        //no schedule file found. this is not normal
        log(LOGLEVEL::ERR, "[check_for_sched_tasks()]: Error opening schedule file: ", sched_path, " while checking for scheduled tasks; aborting");
        delete[] command;
        return 1;
    }
    else {
        fscanf_s(fp, "%s", command, 500); //the cert is always the firs tline
        fgetc(fp); //get the newline
        while (!feof(fp)) {
            if (!feof(fp)) {
                //read date-time config. it starts with " and ends with "
                char* datetime = new char[300];
                datetime[0] = '\0';
                //search for datetime, starting ", then loop until ending "
                int cnt = 0;
                int chr = 0;
                chr = fgetc(fp);//read in the first ", or at least try it
                //printf("%c\n", chr);
                if (chr == '\"') {
                    chr = 0;
                    while (cnt < 295 && chr != '\"') {
                        chr = fgetc(fp); //get a char
                        if (chr != '\"')
                            datetime[cnt] = chr;
                        datetime[cnt + 1] = '\0';
                        cnt++;
                        //printf("scanning...\n");
                    }
                }
                //now we had datetime. we can scan the command and the path now
                fscanf_s(fp, "%s", command, 295); // get the command
                char* path = new char[300];
                path[0] = '\0';
                //search for arg, starting ", then loop until ending "
                cnt = 0;
                chr = 0;
                fgetc(fp); //get th ewhitespoace after the command
                chr = fgetc(fp);//read in the first ", or at least try it

                if (chr == '\"') {
                    chr = 0;
                    while (cnt < 295 && chr != '\"') {
                        chr = fgetc(fp); //get a char
                        if (chr != '\"')
                            path[cnt] = chr;
                        path[cnt + 1] = '\0';
                        cnt++;
                    }
                }
                //now get the taskname
                char* taskname = new char[300];
                taskname[0] = '\0';
                fscanf_s(fp, "%s", taskname, 295); // get the taskname
                fgetc(fp); //get the newline
                //lets check if the command should be executed
                //get the current time
                std::time_t t = std::time(nullptr);
                struct std::tm current_time;
                localtime_s(&current_time, &t);

                //printf("%s\n", datetime);
                if (strcmp(datetime, "") != 0 && strcmp(command, "") != 0 && strcmp(path, "") != 0 && strcmp(taskname, "") != 0) {
                    if (is_task_due(taskname, datetime, current_time)) {
                        //printf("command:%s\n", command);
                        //printf("path:%s\n", path);
                        //now we can build up the command for the queue
                        char* queue_entry = new char[300 * 2 + 5];						//to enshure we have enough space
                        queue_entry[0] = '\0';
                        strcpy_s(queue_entry, 600, command);							//copy the command
                        strcat_s(queue_entry, 600, ";");							//add a ; to seperate command and path
                        strcat_s(queue_entry, 600, path);							//add the path

                        queue_push(queue_entry);
                        delete[] queue_entry;
                    }
                }
                //else {
                //    printf("datetime:%s\n", datetime);
                //    printf("command:%s\n", command);
                //    printf("path:%s\n", path);
                //    printf("taskname:%s\n", taskname);
                //    log(LOGLEVEL::ERR, "[check_for_sched_tasks()]: Error reading schedule file: ", sched_path, " while checking for scheduled tasks (malformat); aborting");
                //}
                delete[] datetime;
                delete[] path;
                delete[] taskname;
            }
        }
    }
    delete[] command;
    fclose(fp);
    return 0;
}
#endif // !LOCAL_SCHEDULE_CPP