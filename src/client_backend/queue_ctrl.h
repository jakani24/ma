#pragma once
#ifndef QUEUE_CTRL_H
#define QUEUE_CTRL_H
#include <string>
int queue_push(const std::string& str);
std::string queue_pop();
int get_queue_size();

#endif // !QUEUE_CTRL_H
