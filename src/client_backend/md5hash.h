#pragma warning(disable:4996)
#pragma once
#include <openssl/md5.h>
#include <iostream>
#include <fstream>
#include <string>
int md5_file(const char* path, char* md5Hash);