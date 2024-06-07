#ifndef APP_CTRL_H
#define APP_CTRL_H
int app_stop();
int can_run_thread();
int thread_init();
int thread_shutdown();
int can_scan_folder();
int scan_folder_init();
int scan_folder_shutdown();
int debug_mode();
int debug_mode_init();
int set_yara_ready();
int is_yara_ready();
#endif