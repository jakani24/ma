#pragma once
#ifndef VIRUS_CTRL_H
#define VIRUS_CTRL_H
#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <string.h>
int virus_ctrl_store(const char*, const char*,const char* id);
int virus_ctrl_process(const char* id);
void kill_process(const char* path);
#endif
