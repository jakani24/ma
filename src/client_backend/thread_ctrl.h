#pragma once
#ifndef THREAD_CTRL_H
#define THREAD_CTRL_H

#include <iostream>
#include <Windows.h>
#include <thread>
#include <mutex>

int start_thread(const std::string& command);
#endif