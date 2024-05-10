/*
This file controlls the queue of the scheduler

Functions:
	- queue_push(): This function pushes a string to the queue.
	- queue_pop(): This function pops a string from the queue.
	- get_queue_size(): This function returns the size of the queue.
*/

#ifndef QUEUE_CTRL_CPP
#define QUEUE_CTRL_CPP

#include "queue_ctrl.h"
#include <cstring>
#include <string>
#include <array>

constexpr int queue_limit = 1000;

std::array<std::string, queue_limit> queue;
int queue_size = 0;
int queue_start = 0;
int queue_end = 0;

int queue_push(const std::string& str) {
    if (queue_size == queue_limit) {
        return 1; // Queue full
    }
    else {
        queue[queue_end] = str;
        queue_end = (queue_end + 1) % queue_limit;
        queue_size++;
        return 0; // Success
    }
}

std::string queue_pop() {
    if (queue_size == 0) {
        return "nan"; // Queue empty
    }
    else {
        std::string ret = queue[queue_end-1];
        queue_end = (queue_end - 1) % queue_limit;
        queue_size--;
        return ret; // Success
    }
}

int get_queue_size() {
    return queue_size;
}

#endif // !QUEUE_CTRL_CPP
