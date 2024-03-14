#include "ui.h"
#include "id.h"
#include <windows.h>
#include "choose_element.h"
HWND hwnd_[100];
int num_hwnd = 0;

void draw_main_page(int window_width, int window_height, HWND hwnd);
void draw_scan_file_page(int window_width, int window_height, HWND hwnd);

void ui_display_page(int page_id,int window_width, int window_height, HWND hwnd) {
	//create one button in middle of screen
	//hwnd_[num_hwnd] = CreateWindowW(L"BUTTON", L"OK",  WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,(window_width-50)/2,(window_height-15)/2, 100, 30, hwnd, (HMENU)ID_STARTUP_BUTTON, NULL, NULL);
	//num_hwnd++;
	switch (page_id) {
		case 0: //main page
			draw_main_page(window_width,window_height,hwnd);
		break;
		case 1: //scan file page
			draw_scan_file_page(window_width, window_height, hwnd);
		break;
	}
}

void ui_clear() {
	//clear all windows
	for (int i = 0; i < num_hwnd; i++) {
		if (hwnd_[i] != NULL) {
			DestroyWindow(hwnd_[i]);
		}
	}
	num_hwnd = 0;
}

void draw_main_page(int window_width, int window_height, HWND hwnd) {
	//draw main page
	hwnd_[num_hwnd] = CreateWindow(L"EDIT",
		L"Welcome to cyberhex\r\n\r\nPlease select an option:\r\n\r\n",
		WS_VISIBLE | WS_CHILD | ES_READONLY | ES_MULTILINE,
		10, 10, window_width - 30, window_height - 200,
		hwnd, (HMENU)0, NULL, NULL);
	num_hwnd++;

	hwnd_[num_hwnd]= CreateWindow(L"BUTTON",
		L"Scan a file",
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		(window_width)/4-100, window_height - 150, 200, 30,
		hwnd, (HMENU)ID_SCAN_FILE_BUTTON, NULL, NULL);
	num_hwnd++;
	hwnd_[num_hwnd] = CreateWindow(L"BUTTON",
		L"Scan a folder",
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		(2* window_width) / 4-100, window_height - 150, 200, 30,
		hwnd, (HMENU)ID_SCAN_FOLDER_BUTTON, NULL, NULL);
	num_hwnd++;
	hwnd_[num_hwnd] = CreateWindow(L"BUTTON",
		L"Settings",
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		(3* window_width) / 4-100, window_height - 150, 200, 30,
		hwnd, (HMENU)ID_SETTINGS_BUTTON, NULL, NULL);
	num_hwnd++;
}

void draw_scan_file_page(int window_width, int window_height, HWND hwnd) {
	//draw scan file page
	std::wstring message = L"File to scan: ";
	std::wstring path=open_file(hwnd);
	if (path == L"cancel") {
		ui_clear();
		ui_display_page(0, window_width, window_height, hwnd);
		return;
	}
	message += path;
	hwnd_[num_hwnd] = CreateWindow(L"EDIT",
				message.c_str(),
				WS_VISIBLE | WS_CHILD | ES_READONLY | ES_MULTILINE,
				10, 10, window_width - 30, window_height - 200,
				hwnd, (HMENU)0, NULL, NULL);
	num_hwnd++;

	hwnd_[num_hwnd] = CreateWindow(L"BUTTON",
				L"Home",
				WS_VISIBLE | WS_CHILD | WS_BORDER,
				10, window_height - 50, 200, 30,
				hwnd, (HMENU)ID_HOME_BUTTON, NULL, NULL);
	num_hwnd++;
}

void update_scan_results(int window_width, int window_height, HWND hwnd,std::wstring& message) {
	//draw scan results
	//get value from the text field of draw_scan_file_page
	//add to thetextfield the string message
	

}