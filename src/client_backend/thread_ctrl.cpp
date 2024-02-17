#ifndef THREAD_CTRL_CPP
#define THREAD_CTRL_CPP
#include "thread_ctrl.h"
#include "log.h"
#include "well_known.h"
#include "scan.h"
#include "app_ctrl.h"
#include "update.h"
void split(char* input,const char delimiter, char* out1, char* out2) {
	//split a string at the delimiter. the delimiter only occurs once. so the first part is out1 and the second part is out2
	int i = 0;
	int j = 0;
	int k = 0;
	while (input[i] != '\0') {
		if (input[i] == delimiter) {
			out1[j] = '\0';
			i++;
			while (input[i] != '\0') {
				out2[k] = input[i];
				i++;
				k++;
			}
			out2[k] = '\0';
			return;
		}
		else {
			out1[j] = input[i];
			i++;
			j++;
		}
	}
}
int start_thread(const char* command) {
	if (can_run_thread()) {
		char* out2 = new char[100]; //for the command
		char* out1 = new char[300]; //for the arguments
		split((char*)command,';', (char*)out1, (char*)out2);
		log(LOGLEVEL::INFO, "[start_thread()]: starting command: ", out1, " with arguments: ",out2);
		//printf("out1: %s\n", out1);
		//printf("out2: %s\n", out2);
		//determine what should be executed
		if (strcmp(out1, "scanfile") == 0) {
			//start a new thread with the scanfile function
			std::thread t1(action_scanfile, out2);
			t1.detach();
		}
		else if (strcmp(out1, "scanfolder") == 0) {
			//start a new thread with the scanfolder function
			std::thread t1(action_scanfolder, out2);
			t1.detach();
		}
		else if (strcmp(out1, "update_settings") == 0) {
			//start a new thread with the scanfolder function
			std::thread t1(action_update_settings);
			t1.detach();
		}

		while (can_run_thread()) {
			//delay a bit, in order to wait until the thread is started
			Sleep(10);
		}
		delete[] out1;
		delete[] out2;
	}
	return 0;
}

#endif