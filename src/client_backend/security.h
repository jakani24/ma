#pragma once
#ifndef SECURITY_H
#define SECURITY_H
#include <string>
#include <iostream>
#include <fstream>
int check_cert(const std::string& cert, const std::string& secrets_path);
std::string get_apikey(const std::string& secrets_path);
std::string get_machineid(const std::string& secrets_path);
#endif // !SECURITY_H