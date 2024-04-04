#pragma once
int check_cert(const char* cert, const char* secrets_path);
char* get_apikey(const char* secrets_path);
char* get_machineid(const char* secrets_path);
char* get_server(const char* secrets_path);