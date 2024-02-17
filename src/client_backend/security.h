#pragma once
#ifndef SECURITY_H
#define SECURITY_H
#include <string>
#include <iostream>
#include <fstream>
int check_cert(const char* cert,const char* secrets_path);
char*get_apikey(const char*secrets_path);
char* get_machineid(const char*secrets_path);
#endif // !SECURITY_H