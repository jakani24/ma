#ifndef SETTINGS_CPP
#define SETTINGS_CPP
#include "settings.h"
#include "well_known.h"
#include "log.h"
int setting_virus_ctrl_virus_found_action = 0;
char*setting_server_server_url = new char[300];
char exluded_folders[100][300];
int excluded_folders_size = 0;

void load_excluded_folders();
int load_settings() {
	FILE* fp;

	if (fopen_s(&fp, SETTINGS_DB, "r")!=0) {
		log(LOGLEVEL::ERR, "[load_settings()]: Could not open settings file. ", SETTINGS_DB);
		return 1;
	}
	else {
		char*settings_cmd=new char[300];
		char*settings_arg= new char[300];
		while (!feof(fp)) {
			fscanf_s(fp, "%s", settings_cmd, 295); // get the command
			//now check which setting it is.
			if (strcmp(settings_cmd, "virus_ctrl:virus_found:action") == 0) {
				fscanf_s(fp, "%s", settings_arg, 295); // get the argument
				if (strcmp(settings_arg, "remove") == 0) {
					setting_virus_ctrl_virus_found_action = 1; //1=remove
				}
				else if (strcmp(settings_arg, "quarantine") == 0) {
					setting_virus_ctrl_virus_found_action = 2; //2=quarantine
				}
				else if (strcmp(settings_arg, "ignore") == 0) {
					setting_virus_ctrl_virus_found_action = 3; //3=ignore
				}
				else if (strcmp(settings_arg, "call_srv") == 0) {
					setting_virus_ctrl_virus_found_action = 4; //4=call_srv <= only call the server and tell it, do not remove or quarantine
				}
			}
			else if (strcmp(settings_cmd, "server:server_url") == 0) {
				fscanf_s(fp, "%s", settings_arg, 295); // get the argument
				strcpy_s(setting_server_server_url, 295, settings_arg);
			}


		}


		delete[] settings_cmd;
		delete[] settings_arg;
	}
	load_excluded_folders();
	return 0;
}

//we have two different get_setting functions. One for int and one for char* return values
int get_setting(const char*setting_name) {
	if (strcmp(setting_name, "virus_ctrl:virus_found:action") == 0) {
		return setting_virus_ctrl_virus_found_action;
	}

	return -1;
}
int get_setting(const char* setting_name,char*out) {
	if (strcmp(setting_name, "server:server_url") == 0) {
		strcpy_s(out, 295, setting_server_server_url);
		return 0;
	}

	return -1;
}

void load_excluded_folders() {
	FILE* fp;
	if (fopen_s(&fp, EXCLUDED_FOLDERS, "r") != 0) {
		log(LOGLEVEL::ERR, "[load_excluded_folders()]: Could not open excluded folders file. ", EXCLUDED_FOLDERS);
		return;
	}
	else {
		char* path = new char[300];
		while (!feof(fp)) {
			//get the path of an excluded folder
			path[0] = '\0';
			//the path is encapsulated with "
			int cnt = 0;
			int chr = 0;
			chr = fgetc(fp);
			if (chr == '\"') {
				chr = 0;
				while (cnt < 295 && chr != '\"') {
					chr = fgetc(fp); //get a char
					if (chr != '\"')
						path[cnt] = chr;
					path[cnt + 1] = '\0';
					cnt++;
				}
				//now add the path to the array
				if (excluded_folders_size < 95) {
					strcpy_s(exluded_folders[excluded_folders_size], 295, path);
					excluded_folders_size++;
				}
				else {
					log(LOGLEVEL::ERR, "[load_excluded_folders()]: Excluded folders array is full. Cannot add more folders.");
				}
			}
			else {
				log(LOGLEVEL::ERR, "[load_excluded_folders()]: Error while processing excluded folders database. Expected \" but got ", chr);
			}
		}
		fclose(fp);
		delete[] path;
	} 
}
bool is_folder_excluded(const char*path) {
	for (int i = 0; i < excluded_folders_size; i++) {
		if (strstr(path,exluded_folders[i]) != 0 and strcmp(exluded_folders[i],"")!=0 and strcmp(exluded_folders[i], " ") != 0 ) {
			return true;
		}
	}
	return false;
}
void print_exclusions() {
	for (int i = 0; i < excluded_folders_size; i++) {
		log(LOGLEVEL::INFO, "[print_exclusions()]: Excluded folder: ", exluded_folders[i]);
	}
}
#endif