#ifndef QUEUE_CTRL_CPP
#define QUEUE_CTRL_CPP
#include "queue_ctrl.h"
#include <string.h>
#define queue_limit 1000
#define command_limit 300
char queue[queue_limit][command_limit];
int queue_size = 0;
int queue_start = 0;
int queue_end = 0;
int queue_push(char* str) {
	if (queue_size == queue_limit) {
		return 1;
	}
	else if (strlen(str) >= command_limit) {
		return 2;
	}
	else {
		strcpy_s(queue[queue_end], str);
		queue_end++;
		queue_size++;
		if (queue_end == queue_limit) {
			queue_end = 0;
		}
		return 0;
	}
}
int queue_pop(char* str) {
	if (queue_size == 0) {
		return 1;
	}
	else {
		strcpy_s(str, 100, queue[queue_start]);
		queue_start++;
		queue_size--;
		if (queue_start == queue_limit) {
			queue_start = 0;
		}
		return 0;
	}
}
int get_queue_size() {
	return queue_size;
}
#endif // !QUEUE_CTRL_CPP
