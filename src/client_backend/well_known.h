#pragma once
#ifndef WELL_KNOWN_H
#define WELL_KNOWN_H
#define MAIN_COM "main_com.txt"	//main communication file for the desktop app
#define MAIN_COM_PATH "C:\\Program Files\\cyberhex\\com\\main_com.txt" //path to the main communication file
#define ANSWER_COM "answer_com.txt" //answer communication file for the desktop app
#define ANSWER_COM_PATH "C:\\Program Files\\cyberhex\\com\\answer_com.txt" //path to the answer communication file

#define SECURE_COM "secure_com.txt" //communication file for the server. Not used right now, but may be used in the future when the server is able to send commands
#define SECURE_COM_PATH "C:\\Program Files\\cyberhex\\secure\\com\\secure_com.txt" //path to the secure communication file

#define SCHED "sched.txt" //scheduler file for the scheduled tasks sent by the server
#define SCHED_PATH "C:\\Program Files\\cyberhex\\secure\\settings\\sched.txt" //path to the scheduler file

#define LOGFILE "C:\\Program Files\\cyberhex\\secure\\log\\log.txt" //path to the log file
#define INFOFILE "C:\\Program Files\\cyberhex\\secure\\log\\info.txt" //path to the info log file
#define WARNFILE "C:\\Program Files\\cyberhex\\secure\\log\\warn.txt" //path to the warning log file
#define ERRORFILE "C:\\Program Files\\cyberhex\\secure\\log\\error.txt" //path to the error log file
#define VIRUSFILE "C:\\Program Files\\cyberhex\\secure\\log\\virus.txt" //path to the virus log file
#define RISKFILE "C:\\Program Files\\cyberhex\\secure\\log\\risk.txt" //path to the risk log file! not used right now
#define PANICFILE "C:\\Program Files\\cyberhex\\secure\\log\\panic.txt" //path to the panic log file
#define SRV_LOGFILE "C:\\Program Files\\cyberhex\\secure\\log\\srv_log.txt" //path to the server log file. only used as backup, if the server would crash


#define FOLDER_DATABASE_DIR "C:\\Program Files\\cyberhex\\secure\\database\\folder" //path to the folder database directory where the hashes are stored

#define APP_DIR "C:\\Program Files\\cyberhex\\secure\\app" //path to the app directory

#define DB_DIR "C:\\Program Files\\cyberhex\\secure\\database" //path to the database directory where the hashes are stored
#define YARA_DB_DIR "C:\\Program Files\\cyberhex\\secure\\yara" //path to the yara database directory where the yara rules are stored

#define VIRUS_CTRL_DB "C:\\Program Files\\cyberhex\\secure\\database\\virus_ctrl_db.txt" //path to the virus control database

#define SETTINGS_DB "C:\\Program Files\\cyberhex\\secure\\settings\\settings_db.txt" //path to the settings database

#define QUARANTINE_PATH "C:\\Program Files\\cyberhex\\secure\\quarantine" //path to the quarantine directory

#define SECRETS "C:\\Program Files\\cyberhex\\secure\\settings\\secrets.txt" //path to the secrets file which containes the api key and the cert of the server

#define PERIODIC_FOLDER_SCAN "C:\\Program Files\\cyberhex\\secure\\database\\folder\\periodic_folder_scan.txt" //path to the periodic folder scan file
#define PERIODIC_FOLDER_SCAN_TEMP_DB "C:\\Program Files\\cyberhex\\secure\\database\\folder\\temp_db.txt" //path to the temporary database file used for multiple things

#define INCLUDED_FOLDERS "C:\\Program Files\\cyberhex\\secure\\settings\\included_folders.txt" //path to the included folders file which stores the included paths
#define EXCLUDED_FOLDERS "C:\\Program Files\\cyberhex\\secure\\settings\\excluded_folders.txt" //path to the excluded folders file which stores the excluded paths
#define DISALLOWED_START_FOLDERS "C:\\Program Files\\cyberhex\\secure\\settings\\disallowed_start_folders.txt" //path to the disallowed start folders file which stores the disallowed start paths

#define JAKACH_UPDATE_SRV "https://jakach.duckdns.org/cyberhex/latest/" //url to the server where the updates are stored, namely the main jakach server
#define CURRENT_VERSION 1 //current version of the app

//hashes for self check
#define APP_HASH "aa" //hash of the app
#define DLL_LIBCRYPTO_HASH "06431d044cf37522e1434019522d11a0" //hash of the libcrypto dll
#define DLL_LIBCURL_HASH "cec7fb7e6cfb46be0740b02fb828900d" //hash of the libcurl dll
#define DLL_MSVCP140_HASH "ebf8072a3c5c586979313f76e503aabf" //hash of the msvcp140 dll
#define DLL_MSVCP140D_HASH "f265bdccc3d8dbc56344c0d900a833bc" //hash of the msvcp140d dll
#define DLL_VCRUNTIME140_HASH "699dd61122d91e80abdfcc396ce0ec10" //hash of the vcruntime140 dll
#define DLL_VCRUNTIME140D_HASH "8e32121bc62367042f6d105df057d6b4" //hash of the vcruntime140d dll
#define DLL_VCRUNTIME140_1D_HASH "75e81353c004a6bea844c212db09e1ed" //hash of the vcruntime140_1d dll
#define DLL_ZLIB1_HASH "a966f1c10844493269521fef54e4cdef" //hash of the zlib1 dll
#define DLL_UCRTBASED_HASH "ffc3b14a9c1280517429e805ed9625b2" //hash of the ucrtbased dll

#define APP_PATH "C:\\Program Files\\cyberhex\\secure\\app\\cyberhex.exe" //path to the app
#define DLL_LIBCRYPTO_PATH "c:\\Program Files\\cyberhex\\secure\\app\\libcrypto-3-x64.dll" //path to the libcrypto dll
#define DLL_LIBCURL_PATH "C:\\Program Files\\cyberhex\\secure\\app\\libcurl.dll" //path to the libcurl dll
#define DLL_MSVCP140_PATH "C:\\Program Files\\cyberhex\\secure\\app\\msvcp140.dll" //path to the msvcp140 dll
#define DLL_MSVCP140D_PATH "C:\\Program Files\\cyberhex\\secure\\app\\msvcp140d.dll" //path to the msvcp140d dll
#define DLL_VCRUNTIME140_PATH "C:\\Program Files\\cyberhex\\secure\\app\\vcruntime140.dll" //path to the vcruntime140 dll
#define DLL_VCRUNTIME140D_PATH "C:\\Program Files\\cyberhex\\secure\\app\\vcruntime140d.dll" //path to the vcruntime140d dll
#define DLL_VCRUNTIME140_1D_PATH "C:\\Program Files\\cyberhex\\secure\\app\\vcruntime140_1d.dll" //path to the vcruntime140_1d dll
#define DLL_ZLIB1_PATH "C:\\Program Files\\cyberhex\\secure\\app\\zlib1.dll" //path to the zlib1 dll
#define DLL_UCRTBASED_PATH "C:\\Program Files\\cyberhex\\secure\\app\\ucrtbased.dll" //path to the ucrtbased dll

#endif // !WELL_KNOWN_H