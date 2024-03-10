#ifndef VIRUS_CTRL_CPP
#define VIRUS_CTRL_CPP
#include "virus_ctrl.h"
#include "well_known.h"
#include "log.h"
#include "settings.h"
#include "connect.h"
#include "security.h"

int virus_ctrl_store( const char*path, const char*hash, const char*id) {
	FILE* fp;
	char *db_path = new char[300];
	strcpy_s(db_path, 295,VIRUS_CTRL_DB);
	strcat_s(db_path, 295, id);
	if (fopen_s(&fp, db_path, "a") != 0) {
		log(LOGLEVEL::ERR, "[virus_ctrl_store()]:Failed to open virus control database to store path of infected file: ",path);
		delete[] db_path;
		return 1;
	}
	else {
		fprintf(fp, "\"%s\" %s\n", path, hash);
		fclose(fp);
		delete[] db_path;
		return 0;
	}
}
int virus_ctrl_process( const char* id) {
	//take actions based on settings.
	//eg delete infected files, quarantine them, etc

	 FILE* fp;
	 char* db_path = new char[300];
	strcpy_s(db_path, 295, VIRUS_CTRL_DB);
	strcat_s(db_path, 295, id);
	if (fopen_s(&fp, db_path, "r") != 0) {
		log(LOGLEVEL::ERR, "[virus_ctrl_process()]:Failed to open virus control database to process it.", db_path);
		delete[] db_path;
		return 1;
	}
	else {
		while (!feof(fp)) {
			//get a fulll path (enclosed with "")
			char* path = new char[300];
			char* hash = new char[300];
			path[0] = '\0';
			//search for starting ", then loop until ending "
			int cnt = 0;
			int chr = 0;
			chr=fgetc(fp);
			if (chr == '\"') {//get the location of the file
				chr = 0;
				while (cnt < 295 && chr != '\"') {
					chr = fgetc(fp); //get a char
					if (chr != '\"')
						path[cnt] = chr;
					path[cnt + 1] = '\0';
					cnt++;
				}
				fscanf_s(fp, "%s", hash, 295); // get the hash of the file
				char* quarantine_path = new char[300];
				char* url = new char[1005];
				char* server_response = new char[100];
				switch (get_setting("virus_ctrl:virus_found:action")) {
				case 1://remove
					if(remove(path)!=0)
						log(LOGLEVEL::ERR, "[virus_ctrl_process()]:Error while removing infected file: ", path," ",hash);
					else
						log(LOGLEVEL::VIRUS, "[virus_ctrl_process()]:Removed Virus: ", path, " ", hash, "");
					break;

				case 2://quarantine
					strcpy_s(quarantine_path, 295, QUARANTINE_PATH);
					strcat_s(quarantine_path, 295, "\\");
					strcat_s(quarantine_path, 295, hash);
					if(rename(path,quarantine_path)!=0)
						log(LOGLEVEL::ERR, "[virus_ctrl_process()]:Error while quarantining infected file: ", path," ",hash);
					else
						log(LOGLEVEL::VIRUS, "[virus_ctrl_process()]:Quarantined Virus: ", path, " ", hash, " to ", quarantine_path);
					break;

				case 3://ignore
					//ignore this file and just continue. but for good measure we should log it
					log(LOGLEVEL::VIRUS, "[virus_ctrl_process()]:Virus found in file: ", path, " ", hash, " but ignored due to settings");
					break;

				case 4://notify
					//call the server and say him that we have found a virus.
					//we shoulkd also log it
					log(LOGLEVEL::VIRUS, "[virus_ctrl_process()]:Virus found in file: ", path, " ", hash, " but only notified due to settings");
					url[0] = '\0';
					if (get_setting("server:server_url", url) == 0 or strcmp(url, "nan") == 0) {
						strcat_s(url, 1000, "/api/php/virus/notify_virus.php?");
						strcat_s(url, 1000, "file=");
						strcat_s(url, 1000, url_encode(path));
						strcat_s(url, 1000, "&hash=");
						strcat_s(url, 1000, hash);
						strcat_s(url, 1000, "&action=");
						strcat_s(url, 1000, "notify");
						strcat_s(url, 1000, "&machine_id=");
						strcat_s(url, 1000, get_machineid(SECRETS));
						strcat_s(url, 1000, "&apikey=");
						strcat_s(url, 1000, get_apikey(SECRETS));
						if (connect_to_srv(url, server_response, 100, get_setting("communication:unsafe_tls")) != 0 or strcmp("wrt_ok", server_response) != 0 )
							log(LOGLEVEL::ERR, "[virus_ctrl_process()]:Error while notifying server about virus: ", path, " ", hash);
					}else {
						log(LOGLEVEL::ERR, "[virus_ctrl_process()]:Error while notifying server about virus: ", path, " ", hash);
					}
					break;

				}
				delete[] quarantine_path;
				delete[] url;
				delete[] server_response;
			}
			//else { creates to many log entrys => entrys are not needed
			//	log(LOGLEVEL::ERR, "[virus_ctrl_process()]:Error while processing virus control database. Expected \" but got ", chr);
			//}
			delete[] path;
			delete[] hash;
		}
		fclose(fp);
	}
	remove(db_path);
	delete[] db_path;
	return 0;
}
#endif