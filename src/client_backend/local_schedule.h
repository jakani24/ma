#pragma once
#ifndef LOCAL_SCHEDULE_H
#define LOCAL_SCHEDULE_H
#include <iostream>
#include <ctime>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
int check_for_sched_tasks(const char* sched_name, const char* sched_path);
void unlock_task(const std::string& task_name);
#endif // !LOCAL_SCHEDULE_H