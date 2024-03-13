#ifndef LOCAL_SCHEDULE_CPP
#define LOCAL_SCHEDULE_CPP

#include "local_schedule.h"
#include "queue_ctrl.h"
#include "log.h"
#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <ctime>
#include <cstdio>

std::map<std::string, bool> task_states;

bool is_valid_field(const std::string& field, int value) {
    if (field == "*") {
        return true;  // Wildcard matches any value
    }

    std::istringstream iss(field);
    std::string token;
    while (std::getline(iss, token, ',')) {
        if (token.find('-') != std::string::npos) {
            int start, end;
            if (sscanf_s(token.c_str(), "%d-%d", &start, &end) == 2 && value >= start && value <= end) {
                return true;
            }
        }
        else {
            int singleValue;
            if (std::istringstream(token) >> singleValue && value == singleValue) {
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

    if (fields.size() < 5) {
        return false;
    }

    if (is_valid_field(fields[0], current_time.tm_min) &&
        is_valid_field(fields[1], current_time.tm_hour) &&
        is_valid_field(fields[2], current_time.tm_mday) &&
        is_valid_field(fields[3], current_time.tm_mon + 1) &&
        is_valid_field(fields[4], current_time.tm_wday + 1)) {

        if (!task_states[task_name]) {
            task_states[task_name] = true;
            return true;
        }
    }
    else {
        task_states[task_name] = false;
    }

    return false;
}

void unlock_task(const std::string& task_name) {
    task_states[task_name] = false;
}

int check_for_sched_tasks(const char* sched_name, const char* sched_path) {
    FILE* fp = nullptr;
    char command[505];
    if (fopen_s(&fp, sched_path, "r") != 0) {
        delete[] command;
        return 1;
    }
    else {
        fscanf_s(fp, "%s", command, _countof(command));
        fgetc(fp);
        while (!feof(fp)) {
            if (!feof(fp)) {
                char datetime[300] = { '\0' };
                int cnt = 0;
                int chr = 0;
                chr = fgetc(fp);
                if (chr == '\"') {
                    chr = 0;
                    while (cnt < 295 && chr != '\"') {
                        chr = fgetc(fp);
                        if (chr != '\"')
                            datetime[cnt] = chr;
                        datetime[cnt + 1] = '\0';
                        cnt++;
                    }
                }

                fscanf_s(fp, "%s", command, _countof(command));
                char path[300] = { '\0' };
                cnt = 0;
                chr = 0;
                fgetc(fp);
                chr = fgetc(fp);
                if (chr == '\"') {
                    chr = 0;
                    while (cnt < 295 && chr != '\"') {
                        chr = fgetc(fp);
                        if (chr != '\"')
                            path[cnt] = chr;
                        path[cnt + 1] = '\0';
                        cnt++;
                    }
                }

                char taskname[300] = { '\0' };
                fscanf_s(fp, "%s", taskname, _countof(taskname));
                fgetc(fp);

                std::time_t t = std::time(nullptr);
                struct std::tm current_time;
                localtime_s(&current_time, &t);

                if (strcmp(datetime, "") != 0 && strcmp(command, "") != 0 && strcmp(taskname, "") != 0) {
                    if (is_task_due(taskname, datetime, current_time)) {
                        char queue_entry[600];
                        queue_entry[0] = '\0';
                        strcpy_s(queue_entry, command);
                        strcat_s(queue_entry, ";");
                        strcat_s(queue_entry, path);
                        log(LOGLEVEL::INFO_NOSEND, "[check_for_sched_tasks()]:Pushing task to queue: ", queue_entry);
                        queue_push(queue_entry);
                    }
                }

            }
        }
    }
    fclose(fp);
    return 0;
}

#endif // !LOCAL_SCHEDULE_CPP
