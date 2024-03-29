#ifndef APP_CTRL_CPP
#define APP_CTRL_CPP
#include "app_ctrl.h"
bool app_stop_ = false;
bool can_run_thread_ = true;
bool can_scan_folder_ = true;
bool debug_mode_ = false;
int app_stop() {
	return app_stop_;
}
int can_run_thread() {
	return can_run_thread_;
}
int thread_init() {
	can_run_thread_ = false;
	return can_run_thread_;
}
int thread_shutdown() {
	can_run_thread_ = true;
	return can_run_thread_;
}
int can_scan_folder() {
	return can_scan_folder_;
}
int scan_folder_init() {
	can_scan_folder_ = false;
	return can_scan_folder_;
}
int scan_folder_shutdown() {
	can_scan_folder_ = true;
	return can_scan_folder_;
}
int debug_mode() {
	return debug_mode_;
}
int debug_mode_init() {
	debug_mode_ = true;
	return debug_mode_;
}
#endif