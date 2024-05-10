#pragma once
#ifndef WELL_KNOWN_H
#define WELL_KNOWN_H
#define MAIN_COM "main_com.txt"
#define MAIN_COM_PATH "C:\\Program Files\\cyberhex\\com\\main_com.txt"
#define ANSWER_COM "answer_com.txt"
#define ANSWER_COM_PATH "C:\\Program Files\\cyberhex\\com\\answer_com.txt"

#define SECURE_COM "secure_com.txt"
#define SECURE_COM_PATH "C:\\Program Files\\cyberhex\\secure\\com\\secure_com.txt"

#define SCHED "sched.txt"
#define SCHED_PATH "C:\\Program Files\\cyberhex\\secure\\settings\\sched.txt"

#define LOGFILE "C:\\Program Files\\cyberhex\\secure\\log\\log.txt"
#define INFOFILE "C:\\Program Files\\cyberhex\\secure\\log\\info.txt"
#define WARNFILE "C:\\Program Files\\cyberhex\\secure\\log\\warn.txt"
#define ERRORFILE "C:\\Program Files\\cyberhex\\secure\\log\\error.txt"
#define VIRUSFILE "C:\\Program Files\\cyberhex\\secure\\log\\virus.txt"
#define RISKFILE "C:\\Program Files\\cyberhex\\secure\\log\\risk.txt"
#define PANICFILE "C:\\Program Files\\cyberhex\\secure\\log\\panic.txt"
#define SRV_LOGFILE "C:\\Program Files\\cyberhex\\secure\\log\\srv_log.txt"


#define FOLDER_DATABASE_DIR "C:\\Program Files\\cyberhex\\secure\\database\\folder"

#define APP_DIR "C:\\Program Files\\cyberhex\\secure\\app"

#define DB_DIR "C:\\Program Files\\cyberhex\\secure\\database"
#define YARA_DB_DIR "C:\\Program Files\\cyberhex\\secure\\yara"

#define VIRUS_CTRL_DB "C:\\Program Files\\cyberhex\\secure\\database\\virus_ctrl_db.txt"

#define SETTINGS_DB "C:\\Program Files\\cyberhex\\secure\\settings\\settings_db.txt"

#define QUARANTINE_PATH "C:\\Program Files\\cyberhex\\secure\\quarantine"

#define SECRETS "C:\\Program Files\\cyberhex\\secure\\settings\\secrets.txt"

#define PERIODIC_FOLDER_SCAN "C:\\Program Files\\cyberhex\\secure\\database\\folder\\periodic_folder_scan.txt"
#define PERIODIC_FOLDER_SCAN_TEMP_DB "C:\\Program Files\\cyberhex\\secure\\database\\folder\\temp_db.txt"

#define INCLUDED_FOLDERS "C:\\Program Files\\cyberhex\\secure\\settings\\included_folders.txt"
#define EXCLUDED_FOLDERS "C:\\Program Files\\cyberhex\\secure\\settings\\excluded_folders.txt"
#define DISALLOWED_START_FOLDERS "C:\\Program Files\\cyberhex\\secure\\settings\\disallowed_start_folders.txt"

#define JAKACH_UPDATE_SRV "https://jakach.duckdns.org/cyberhex/latest/"
#define CURRENT_VERSION 1

//hashes for self check
#define APP_HASH "aa"
#define DLL_LIBCRYPTO_HASH "06431d044cf37522e1434019522d11a0"
#define DLL_LIBCURL_HASH "cec7fb7e6cfb46be0740b02fb828900d"
#define DLL_MSVCP140_HASH "ebf8072a3c5c586979313f76e503aabf"
#define DLL_MSVCP140D_HASH "f265bdccc3d8dbc56344c0d900a833bc"
#define DLL_VCRUNTIME140_HASH "699dd61122d91e80abdfcc396ce0ec10"
#define DLL_VCRUNTIME140D_HASH "8e32121bc62367042f6d105df057d6b4"
#define DLL_VCRUNTIME140_1D_HASH "75e81353c004a6bea844c212db09e1ed"
#define DLL_ZLIB1_HASH "a966f1c10844493269521fef54e4cdef"
#define DLL_UCRTBASED_HASH "ffc3b14a9c1280517429e805ed9625b2"

#define APP_PATH "C:\\Program Files\\cyberhex\\secure\\app\\cyberhex.exe"
#define DLL_LIBCRYPTO_PATH "c:\\Program Files\\cyberhex\\secure\\app\\libcrypto-3-x64.dll"
#define DLL_LIBCURL_PATH "C:\\Program Files\\cyberhex\\secure\\app\\libcurl.dll"
#define DLL_MSVCP140_PATH "C:\\Program Files\\cyberhex\\secure\\app\\msvcp140.dll"
#define DLL_MSVCP140D_PATH "C:\\Program Files\\cyberhex\\secure\\app\\msvcp140d.dll"
#define DLL_VCRUNTIME140_PATH "C:\\Program Files\\cyberhex\\secure\\app\\vcruntime140.dll"
#define DLL_VCRUNTIME140D_PATH "C:\\Program Files\\cyberhex\\secure\\app\\vcruntime140d.dll"
#define DLL_VCRUNTIME140_1D_PATH "C:\\Program Files\\cyberhex\\secure\\app\\vcruntime140_1d.dll"
#define DLL_ZLIB1_PATH "C:\\Program Files\\cyberhex\\secure\\app\\zlib1.dll"
#define DLL_UCRTBASED_PATH "C:\\Program Files\\cyberhex\\secure\\app\\ucrtbased.dll"

#endif // !WELL_KNOWN_H