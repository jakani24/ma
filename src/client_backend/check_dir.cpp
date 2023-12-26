#ifndef CHECK_DIR_CPP
#define CHECK_DIR_CPP
#include "check_dir.h"
#include "well_known.h"
#include "log.h"
#include "md5hash.h"
#include "scan.h"
#include "app_ctrl.h"
#include "virus_ctrl.h"
#include "scan.h"
#include "settings.h"
/* old implementation. used up a lot of resource and did not work properly.
void add_to_temp_db(const char*hash) {
	//PERIODIC_FOLDER_SCAN_TEMP_DB
	FILE*fp;
	if (fopen_s(&fp, PERIODIC_FOLDER_SCAN_TEMP_DB, "a") != 0) {
		log(LOGLEVEL::ERR, "[add_to_temp_db()]: Error opening temp db: ", PERIODIC_FOLDER_SCAN_TEMP_DB);
		return;
	}
	else {
		//log(LOGLEVEL::INFO, "[add_to_temp_db()]: Adding hash: ", hash, " to temp db: ", PERIODIC_FOLDER_SCAN_TEMP_DB);
		fprintf_s(fp, "%s\n", hash);
		fclose(fp);
	}
}
void scan_folder_recursive(const std::string& directory, int thread_id,const std::string&db_file) {
    std::string search_path = directory + "\\*.*";
    WIN32_FIND_DATA find_file_data;
    HANDLE hFind = FindFirstFile(search_path.c_str(), &find_file_data);

    if (hFind == INVALID_HANDLE_VALUE) {
        //std::cerr << "Error opening directory: " << directory << std::endl;
		log(LOGLEVEL::ERR, "[scan_folder_recursive()]: Error opening directory: ", directory ," while scanning folder for new files");
        return;
    }

    do {
        if (strcmp(find_file_data.cFileName, ".") == 0 || strcmp(find_file_data.cFileName, "..") == 0) {
            continue; // Skip the current and parent directories
        }


        std::string full_path = directory + "\\" + find_file_data.cFileName;
        if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // If it's a directory, recurse into it
            scan_folder_recursive(full_path, thread_id,db_file);
        }
        else {
            // If it's a file, check if it is in db, else scan it and add it to db
			char*hash = new char[300];
            md5_file(full_path.c_str(), hash);
			//now find hash in db
			if (scan_hash(db_file.c_str(), hash)==1) {
				//file is allready in db, skipping
				//in order to not keep hashes that are not present anymore in the db, we have to write this hash into a temp dir, which is at the end copied into the main db
				add_to_temp_db(hash);
			}
			else {
				//scan the file and add it to db
				//scan for virus

				switch (scan_hash(hash)) {
				case 1:
					//virus found
					//log it
					log(LOGLEVEL::VIRUS, "[scan_folder_recursive()]: Virus found in file: ", full_path, " while scanning ", directory, " for new files");
					//virus_ctrl_store(full_path.c_str(), hash, "fs");
					break;
				case 2:
					//error
					log(LOGLEVEL::ERR, "[scan_folder_recursive()]: Error while scanning file: ", full_path, " while scanning ", directory, " for new files");
					break;
				default:
					//not a virus
					add_to_temp_db(hash);
					break;
				}
			}

        }
    } while (FindNextFile(hFind, &find_file_data) != 0);

    FindClose(hFind);
}
int check_scan_dir(char*dirpath,char*dirname) {
	if (can_scan_folder()) {
		scan_folder_init();
		//FOLDER_DATABASE_DIR <= the folder where the database for folder checking is stored
		//the database is stored in the following format:
		/* a file per folder (and its subfolders)
			in this db file the hashes of all the files in the folder (and its subfolders) are stored
			If a file is detected, which hash is not known, it gets scanned and added to the db. if a hash is inside the db, which is not present in the folder, the hash is rempved from the db

		*/
		/*
		FILE* fp;
		char* path = new char[300];
		path[0] = '\0';
		//build up the path for the db file.
		strcpy_s(path, 295, FOLDER_DATABASE_DIR);
		strcat_s(path, 295, "\\");
		strcat_s(path, 295, dirname);
		strcat_s(path, 295, ".jdbf");

		//check if the file exists. else we cannot scan the folder
		if ((fopen_s(&fp, path, "r")) != 0) {
			log(LOGLEVEL::ERR, "[check_scan_dir()]: Error opening database: ", path, " while scanning folder for new files; aborting");
			//try to create the file
			if (fopen_s(&fp, path, "w") != 0) {
				log(LOGLEVEL::ERR, "[check_scan_dir()]: Error creating new database: ", path, " while scanning folder for new files; aborting");
			}
			else {
				fprintf_s(fp, "%s\n", "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");//write A 32 times A into the file. So the algorithm ha ssomething to map into memory. else it might throw an error
				fclose(fp);
			}
			scan_folder_shutdown();
			return 1;
		}
		else {
			fclose(fp);
			//process the files of the folder.
			//first: calculate hash of file
			//second: check if hash is in db
			//if not, scan file and add it to db (only add it if it is not detected as a virus)
			scan_folder_recursive(dirpath, 0,path);
			//process the found viruses
			virus_ctrl_process("fs");
		}
		delete[] path;
		scan_folder_shutdown();
		return 0;
	}
	else {
		
		//there is already a folder scan happening
	}
}

*/
bool is_directory(const std::string& path) {
    DWORD attributes = GetFileAttributes(path.c_str());

    if (attributes == INVALID_FILE_ATTRIBUTES) {
        // Handle the error, e.g., by printing an error message
        return false;
    }

    return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}
void process_changes(const FILE_NOTIFY_INFORMATION* pInfo) {
    // Allocate a buffer for the file name and copy the content
    std::wstring fileName(pInfo->FileName, pInfo->FileNameLength / sizeof(wchar_t));

    //convert wstring to string
    std::string filename_str(fileName.begin(), fileName.end());
    filename_str = "c:\\" + filename_str;
    //scan the file and send it to virus_ctrl if it is a virus and then process it
    std::transform(filename_str.begin(), filename_str.end(), filename_str.begin(), ::tolower);
    if (is_folder_excluded(filename_str.c_str()) or is_directory(filename_str.c_str())) {
		//dont scan excluded files or folders
		return;
    }
    else {
        std::thread scan_thread(action_scanfile_t, filename_str);
        scan_thread.detach();
    }
    log(LOGLEVEL::INFO, "[process_changes()]: File change: ", filename_str.c_str(), " while monitoring directory for changes");
}
/* this was the old algorithm. it was slower and used up more resources, because it used a database to track which files have been modified instead of using the windows internal functions
void monitor_directory(LPCSTR directory) {
    // Open the directory for monitoring
    HANDLE hDir = CreateFile(
        directory,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (hDir == INVALID_HANDLE_VALUE) {
        log(LOGLEVEL::ERR, "[monitor_directory()]: Error opening directory: ", directory, " while monitoring directory for changes");
        return;
    }

    // Create a buffer for file change notifications
    constexpr DWORD bufferSize = 4096;
    BYTE buffer[bufferSize];

    // Monitor the directory for changes
    OVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (ReadDirectoryChangesW(
        hDir,
        buffer,
        bufferSize,
        TRUE,
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE,
        NULL,
        &overlapped,
        NULL) == 0) {
        log(LOGLEVEL::ERR, "[monitor_directory()]: Error reading directory changes: ", GetLastError(), " while monitoring directory for changes");
        CloseHandle(hDir);
        return;
    }

    log(LOGLEVEL::INFO, "[monitor_directory()]: Monitoring directory: ", directory, " for changes");
    //print_exclusions();

    // Wait for changes
    while (true) {
        DWORD bytesReturned;
        if (GetOverlappedResult(hDir, &overlapped, &bytesReturned, TRUE)) {
            // Process the changes in the buffer
            FILE_NOTIFY_INFORMATION* pInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);

            do {
                process_changes(pInfo);

                pInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
                    reinterpret_cast<BYTE*>(pInfo) + pInfo->NextEntryOffset);

            } while (pInfo->NextEntryOffset != 0);

            // Reset the event for the next wait
            ResetEvent(overlapped.hEvent);

            // Continue monitoring
            if (ReadDirectoryChangesW(
                hDir,
                buffer,
                bufferSize,
                TRUE,
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE,
                NULL,
                &overlapped,
                NULL) == 0) {
                log(LOGLEVEL::ERR, "[monitor_directory()]: Error reading directory changes: ", GetLastError(), " while monitoring directory for changes");
                break;
            }
        }
        else {
            DWORD error = GetLastError();
            if (error != ERROR_IO_PENDING) {
                log(LOGLEVEL::ERR, "[monitor_directory()]: Error reading directory changes: ", error, " while monitoring directory for changes");
                break;
            }
        }

        // Wait for the event to be signaled (infinite timeout)
        WaitForSingleObject(overlapped.hEvent, INFINITE);
    }

    // Cleanup
    CloseHandle(overlapped.hEvent);
    CloseHandle(hDir);
}
*/

void monitor_directory(LPCSTR directory) {
    // Open the directory for monitoring
    HANDLE hDir = CreateFile(
        directory,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (hDir == INVALID_HANDLE_VALUE) {
        log(LOGLEVEL::ERR, "[monitor_directory()]: Error opening directory: ", directory, " while monitoring directory for changes");
        return;
    }

    // Create a buffer for file change notifications
    constexpr DWORD bufferSize = 4096;
    BYTE buffer[bufferSize];

    // Monitor the directory for changes
    OVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
    memset(buffer, 0, bufferSize);
    overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (ReadDirectoryChangesW(
        hDir,
        buffer,
        bufferSize,
        TRUE,
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
        NULL,
        &overlapped,
        NULL) == 0) {
        log(LOGLEVEL::ERR, "[monitor_directory()]: Error reading directory changes: ", GetLastError(), " while monitoring directory for changes");
        CloseHandle(hDir);
        return;
    }

    log(LOGLEVEL::INFO, "[monitor_directory()]: Monitoring directory: ", directory, " for changes");

    // Wait for changes
    while (!app_stop()) {
        DWORD bytesReturned;
        DWORD waitStatus = WaitForSingleObject(overlapped.hEvent, INFINITE);

        if (waitStatus == WAIT_OBJECT_0) {
            if (GetOverlappedResult(hDir, &overlapped, &bytesReturned, FALSE)) {
                // Process the changes in the buffer
                FILE_NOTIFY_INFORMATION* pInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);

                do {
                    process_changes(pInfo);

                    pInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
                        reinterpret_cast<BYTE*>(pInfo) + pInfo->NextEntryOffset);

                } while (pInfo->NextEntryOffset != 0);

                // Reset the event for the next wait
                ResetEvent(overlapped.hEvent);
                memset(buffer, 0, bufferSize);

                // Continue monitoring
                if (ReadDirectoryChangesW(
                    hDir,
                    buffer,
                    bufferSize,
                    TRUE,
                    FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
                    NULL,
                    &overlapped,
                    NULL) == 0) {
                    log(LOGLEVEL::ERR, "[monitor_directory()]: Error reading directory changes: ", GetLastError(), " while monitoring directory for changes");
                    break;
                }
            }
            else {
                log(LOGLEVEL::ERR, "[monitor_directory()]: GetOverlappedResult failed: ", GetLastError());
                break;
            }
        }
        else {
            log(LOGLEVEL::ERR, "[monitor_directory()]: WaitForSingleObject failed: ", GetLastError());
            break;
        }
    }

    // Cleanup
    CloseHandle(overlapped.hEvent);
    CloseHandle(hDir);
}

void folder_scanner() {

	//we are in a completely seperate thread then the main thread; unlimited resources wuhuii
	FILE*fp;
	char* path = new char[300];
	char* foldername = new char[300];
	
	//start the watch dir function used to monitor the dir for new files
    monitor_directory("C:\\");


	delete[] path;
	delete[] foldername;
}

#endif // !CHECK_DIR_CPP