#pragma once
#include <windows.h>
#include <sddl.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <windows.h>
#include <sddl.h>
#include <stdio.h>
int protect_file(char* path);
int unprotect_file(char* path);