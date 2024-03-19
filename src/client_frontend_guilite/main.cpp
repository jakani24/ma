#include <nanogui/nanogui.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <iostream>
#include <shlobj.h>
#include <windows.h>
#include <locale>
#include <codecvt>
#include <fstream>

#include "../client_backend/well_known.h"
#include "connect.h"
// Function to simulate file scanning
void scan_file(nanogui::Screen* screen, nanogui::Widget* contentWidget, const std::string& filePath) {
    // Remove the answer file
    std::remove(ANSWER_COM_PATH);
    // Display the scanned file path in the window
    nanogui::Label* lineLabel1 = new nanogui::Label(contentWidget, "Scanning file: " + filePath + "\n");
    screen->performLayout();
    bool answered = false;
    // Write command into com file
    printf("%d\n",send_to_pipe("scanfile \"" + filePath + "\""));
    while (!answered) {
        // Wait for answer in file
        std::ifstream inputFile(ANSWER_COM_PATH);
        // The structure of the answer file is as follows:
        // found/not_found
        // filepath
        // hash
        // action_taken/no_action_taken
        if (inputFile.is_open()) {
            std::string status, scannedFilePath, hash, action;
            if (inputFile >> status) {
                inputFile.ignore(1); // Ignore space
                inputFile.ignore(1); // Ignore starting double quote
                if (status == "found" || status == "not_found") {
                    std::getline(inputFile, scannedFilePath, '\"'); // Read until closing double quote
                    inputFile.ignore(1); // Ignore space between filepath and hash
                    inputFile.ignore(1); // Ignore starting double quote
                    std::getline(inputFile, hash, ' '); // Read until space
                    std::getline(inputFile, action); // Read until end of line

                    answered = true;

                    if (status == "found") {
                        nanogui::Label* lineLabel3 = new nanogui::Label(contentWidget, "Virus found in file: " + scannedFilePath + "\n");
                        nanogui::Label* lineLabel4 = new nanogui::Label(contentWidget, "File: " + scannedFilePath + " is infected\n");
                        nanogui::Label* lineLabel5 = new nanogui::Label(contentWidget, "Hash: " + hash + "\n");
                        nanogui::Label* lineLabel6 = new nanogui::Label(contentWidget, "Action taken: " + action + "\n");
                    }
                    else {
                        nanogui::Label* lineLabel7 = new nanogui::Label(contentWidget, "No virus found in file: " + scannedFilePath + "\n");
                        nanogui::Label* lineLabel8 = new nanogui::Label(contentWidget, "File: " + scannedFilePath + " is not infected\n");
                    }
                    nanogui::Label* lineLabel9 = new nanogui::Label(contentWidget, "------------------------------------------");
                    screen->performLayout();
                }
            }
            else {
                answered = true;
                nanogui::Label* lineLabel10 = new nanogui::Label(contentWidget, "Error: Unable to talk to daemon!\n");
                nanogui::Label* lineLabel11 = new nanogui::Label(contentWidget, "------------------------------------------");
                screen->performLayout();
            }
            inputFile.close();
            std::remove(ANSWER_COM_PATH);
        }
        // Wait for 1 second before checking again
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    // Remove the answer file
    std::remove(ANSWER_COM_PATH);
}

// Function to simulate folder scanning
void scan_folder(nanogui::Screen* screen, nanogui::Widget* contentWidget, const std::string& filePath) {
    // Remove the answer file
    std::remove(ANSWER_COM_PATH);
    // Display the scanned folder path in the window
    nanogui::Label* lineLabel1 = new nanogui::Label(contentWidget, "Scanning folder: " + filePath + "\n");
    screen->performLayout();
    bool answered = false;
    // Write command into com file
    std::ofstream outputFile(MAIN_COM_PATH);
    if (outputFile.is_open()) {
        outputFile << "scanfolder \"" << filePath << "\"";
        outputFile.close();
    }
    else {
        nanogui::Label* lineLabel2 = new nanogui::Label(contentWidget, "Error: Unable to talk to daemon!\n");
        screen->performLayout();
        return;
    }
    while (!answered) {
        // Wait for answer in file
        std::ifstream inputFile(ANSWER_COM_PATH);
        // The structure of the answer file is as follows:
        // found/not_found
        // filepath
        // hash
        // action_taken/no_action_taken
        if (inputFile.is_open()) {
            std::string status, scannedFilePath, hash, action;
            while (!inputFile.eof()) {
                if (inputFile >> status) {
                    inputFile.ignore(1); // Ignore space
                    inputFile.ignore(1); // Ignore starting double quote
                    if (status == "found" || status == "not_found") {
                        std::getline(inputFile, scannedFilePath, '\"'); // Read until closing double quote
                        inputFile.ignore(1); // Ignore space between filepath and hash
                        inputFile.ignore(1); // Ignore starting double quote
                        std::getline(inputFile, hash, ' '); // Read until space
                        std::getline(inputFile, action); // Read until end of line

                        //answered = true;

                        if (status == "found") {
                            nanogui::Label* lineLabel3 = new nanogui::Label(contentWidget, "Virus found in file: " + scannedFilePath + "\n");
                            nanogui::Label* lineLabel4 = new nanogui::Label(contentWidget, "File: " + scannedFilePath + " is infected\n");
                            nanogui::Label* lineLabel5 = new nanogui::Label(contentWidget, "Hash: " + hash + "\n");
                            nanogui::Label* lineLabel6 = new nanogui::Label(contentWidget, "Action taken: " + action + "\n");
                        }
                        else {
                            nanogui::Label* lineLabel7 = new nanogui::Label(contentWidget, "No virus found in file: " + scannedFilePath + "\n");
                            nanogui::Label* lineLabel8 = new nanogui::Label(contentWidget, "File: " + scannedFilePath + " is not infected\n");
                        }
                        nanogui::Label* lineLabel9 = new nanogui::Label(contentWidget, "------------------------------------------");
                        screen->performLayout();
                    }
                    if (status == "end")
                        answered = true;
                }
                else {
                    //answered = true;
                    nanogui::Label* lineLabel10 = new nanogui::Label(contentWidget, "Error: Unable to talk to daemon!\n");
                    nanogui::Label* lineLabel11 = new nanogui::Label(contentWidget, "------------------------------------------");
                    screen->performLayout();
                }
            }
            inputFile.close();
        }
        // Wait for 1 second before checking again
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    // Remove the answer file
    std::remove(ANSWER_COM_PATH);
}
std::string getFolderPath() {
    std::string selectedFolderPath;

    // Initialize COM
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    // Display the folder picker dialog
    BROWSEINFO browseInfo = { 0 };
    TCHAR selectedPath[MAX_PATH];
    browseInfo.hwndOwner = NULL; // Use the desktop window as the owner
    browseInfo.pidlRoot = NULL;  // Start from the desktop
    browseInfo.pszDisplayName = selectedPath;
    browseInfo.lpszTitle = TEXT("Select a folder");
    browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    LPITEMIDLIST pidlSelected = SHBrowseForFolder(&browseInfo);
    if (pidlSelected != NULL) {
        SHGetPathFromIDList(pidlSelected, selectedPath);

        // Convert TCHAR array to std::string
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        selectedFolderPath = converter.to_bytes(selectedPath);

        // Free the PIDL
        IMalloc* pMalloc;
        if (SUCCEEDED(SHGetMalloc(&pMalloc))) {
            pMalloc->Free(pidlSelected);
            pMalloc->Release();
        }
    }

    // Uninitialize COM
    CoUninitialize();

    return selectedFolderPath;
}
int main() {
    // Initialize NanoGUI
    nanogui::init();

    // Create a NanoGUI screen
    nanogui::Screen screen(nanogui::Vector2i(800, 600), "Cyberhex endpoint protection");
    // Create a layout for the buttons
    nanogui::Widget* buttonWidget = new nanogui::Widget(&screen);
    nanogui::BoxLayout* buttonLayout = new nanogui::BoxLayout(nanogui::Orientation::Vertical,
        nanogui::Alignment::Middle,
        5, 5);
    buttonWidget->setLayout(buttonLayout);

    // Create a widget inside the scroll panel to hold the content

    nanogui::VScrollPanel* scrollPanel = new nanogui::VScrollPanel(&screen);
    scrollPanel->setLayout(new nanogui::GroupLayout(10));
    nanogui::Widget* contentWidget = new nanogui::Widget(scrollPanel);
    contentWidget->setLayout(new nanogui::GroupLayout(10));
    scrollPanel->setFixedSize(nanogui::Vector2i(750, 450));



    // Create a button for scanning a file
    nanogui::Button* scanFileButton = new nanogui::Button(buttonWidget, "Scan File");
    scanFileButton->setFixedWidth(150);
    scanFileButton->setCallback([&] {
        // Open file dialog to select a file
        std::string selectedFile = nanogui::file_dialog(
            { {"*", "All Files"} },
            false
        );
        if (!selectedFile.empty()) {
            // Call scan_file function in a separate thread
            std::thread(scan_file, &screen, contentWidget, selectedFile).detach();
        }
        });

    // Create a button for scanning a folder
    nanogui::Button* scanFolderButton = new nanogui::Button(buttonWidget, "Scan Folder");
    scanFolderButton->setFixedWidth(150);
    scanFolderButton->setCallback([&] {
        // Open file dialog to select a folder
        std::string selectedFolder = getFolderPath();
        if (!selectedFolder.empty()) {
            // Call scan_folder function in a separate thread
            std::thread(scan_folder, &screen, contentWidget, selectedFolder).detach();
        }
        });

    // Set the layout for the main screen
    nanogui::BoxLayout* layout = new nanogui::BoxLayout(nanogui::Orientation::Vertical,
        nanogui::Alignment::Minimum,
        10, 10);



    // Add 100 lines to the content widget
    //for (int i = 0; i < 100; ++i) {
    //    nanogui::Label* lineLabel = new nanogui::Label(contentWidget, "Line " + std::to_string(i));
    //    lineLabel->setFixedWidth(200);
    //}

    // Scroll panel will automatically take the size of its content
    //scrollPanel->setFixedHeight(300);

    //screen.setVisible(true);


    screen.setLayout(layout);


    // Draw NanoGUI
    screen.performLayout();
    screen.setVisible(true);

    // Main event loop
    nanogui::mainloop();

    // Shutdown NanoGUI
    nanogui::shutdown();

    return 0;
}