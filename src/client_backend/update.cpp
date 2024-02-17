#ifndef UPDATE_CPP
#define UPDATE_CPP
#include "update.h"
#include "log.h"
#include "connect.h"
#include "settings.h"
#include "security.h"

int update_db(const std::string& folder_path) {
	//download the databases from the server
	for (char firstChar = '0'; firstChar <= 'f'; ++firstChar) {
		for (char secondChar = '0'; secondChar <= 'f'; ++secondChar) {
			// Ensure that the characters are valid hexadecimal digits
			if (!std::isxdigit(firstChar) || !std::isxdigit(secondChar) or std::isupper(firstChar) or std::isupper(secondChar)) {
				continue;
			}

			// Create the filename based on the naming convention
			std::string file_path = folder_path + "\\" + firstChar + secondChar + ".jdbf";
			std::string file_name = firstChar + secondChar + ".jdbf";
			//create the strings to download the files
			char*url=new char[300];
			char*output_path=new char[300];
			get_setting("server:server_url", url);
			strcat_s(url, 295,"/database/");
			strcat_s(url, 295,file_name.c_str() );
			strcpy_s(output_path, 295, file_path.c_str());

			int res = download_file_from_srv(url, output_path);
			if (res != 0) {
				log(LOGLEVEL::ERR, "[update_db()]: Error downloading database file from server", url);
				return 1;
			}


			delete[] url;
			delete[] output_path;

		}
	}
	return 0;
}
int update_settings(const char*settings_type) {
	//create the strings to download the files
	char* url = new char[1000];
	get_setting("server:server_url", url);
	strcat_s(url, 1000, "/api/php/settings/get_settings.php?");//need to add machine_id and apikey
	strcat_s(url, 1000, settings_type);
	strcat_s(url, 1000, "&machine_id=");
	strcat_s(url, 1000, get_machineid(SECRETS));
	strcat_s(url, 1000, "&apikey=");
	strcat_s(url, 1000, get_apikey(SECRETS));
	int res = 1;
	if(strcmp(settings_type,"settings")==0)
		res = download_file_from_srv(url, SETTINGS_DB);
	else if (strcmp(settings_type, "rtp_included") == 0)
		res = download_file_from_srv(url, INCLUDED_FOLDERS);
	else if (strcmp(settings_type, "rtp_excluded") == 0)
		res = download_file_from_srv(url, EXCLUDED_FOLDERS);
	else if (strcmp(settings_type, "sched") == 0)
		res = download_file_from_srv(url, SCHED_PATH);
	//int res = 0;
	if (res != 0) {
		log(LOGLEVEL::ERR, "[update_settings()]: Error downloading settings database file from server", url, " ERROR:",res);
		return 1;
	}

	delete[] url;
	return 0;
}
int action_update_settings() {
	//update the settings
	int err = 0;
	if (update_settings("settings") != 0) {
		err= 1;
	}
	//update the included folders
	if (update_settings("rtp_included") != 0) {
		err= 2;
	}
	//update the excluded folders
	if (update_settings("rtp_excluded") != 0) {
		err= 3;
	}
	//update the schedule
	if (update_settings("sched") != 0) {
		err= 4;
	}
	return err;
}
#endif